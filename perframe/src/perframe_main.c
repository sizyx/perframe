/*
 * File: perframe_main.c
 * Author: sizyx@163.com
 * Description:
 *   this is the source file of main performance frame
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "apr.h"
#include "apr_time.h"
#include "apr_thread_proc.h"
#include "apr_thread_mutex.h"
#include "apr_strings.h"
#include "apr_tables.h"
#include "perframe.h"

static apr_array_header_t *g_test_cases = NULL;

typedef struct thread_param_t {
    int                 thread_no; // thread number
    apr_thread_t       *thread_handle;
    perf_testcase_t    *testcase;
    void               *testcase_ctx;
    apr_byte_t          endflag;   // thread wether end flag
    apr_thread_mutex_t *mutex;     // therad mutex
    apr_int64_t         okcnt;     // success count
    apr_int64_t         errcnt;    // error count
} thread_param_t;

static thread_param_t g_threads[1024] = { { 0 } };
static apr_time_t     g_start_time = 0;

static int  g_thread_count = 1;         // thread count, default 1
static int  g_loop_count = 1;           // loop count per thread, default 1
static int  g_duration_seconds = 0L;    // duration time in seconds, default 0
static apr_pool_t *g_pool = NULL;

// print result thread
static void* APR_THREAD_FUNC print_worker(apr_thread_t *thread, void *data)
{
    perf_testcase_t *testcase = (perf_testcase_t*)data;
    int i;

    apr_sleep(apr_time_from_msec(1));

    do {
        apr_int64_t ok_cnt = 0, err_cnt = 0;
        void *cover_print_ctx;
        apr_time_t elapsed;
        int all_thread_end = 1;

        for (i = 0; i < g_thread_count; ++i) {
            if (g_threads[i].mutex) {
                apr_thread_mutex_lock(g_threads[i].mutex);
            }
            ok_cnt += g_threads[i].okcnt;
            err_cnt += g_threads[i].errcnt;
            if (g_threads[i].mutex) {
                apr_thread_mutex_unlock(g_threads[i].mutex);
            }
        }

        elapsed = apr_time_now() - g_start_time;
        cover_print_ctx = perf_cover_print_start();

        perf_printf("elapsed time:\t\t");
        perf_green_printf("%"APR_TIME_T_FMT".%06"APR_TIME_T_FMT"s\n",
            apr_time_sec(elapsed), apr_time_usec(elapsed));
        perf_cover_print_add_lines(cover_print_ctx, 1);

        if (elapsed == 0) {
            apr_sleep(1);
            continue;
        }

        perf_printf("total ok count:\t\t");
        perf_green_printf("%"APR_INT64_T_FMT"\n", ok_cnt);
        perf_cover_print_add_lines(cover_print_ctx, 1);

        {
            double avg = (ok_cnt * 1.0) / apr_time_sec(elapsed);

            perf_printf("average speed:\t\t");
            perf_green_printf("%.3f times/s\n", avg);
            perf_cover_print_add_lines(cover_print_ctx, 1);
        }

        if (testcase->stype == e_speed_throughput) {
            apr_int64_t totalbytes = ok_cnt * testcase->block_size;
            double totalMbits = (totalbytes * 8.0) / (1024 * 1024);
            double throughput = totalMbits * APR_USEC_PER_SEC / elapsed;

            perf_printf("average throughput:\t");
            perf_green_printf("%.3fMbps\n", throughput);
            perf_cover_print_add_lines(cover_print_ctx, 1);
        }

        if (err_cnt > 0) {
            perf_printf("total error count:\t");
            perf_red_printf("%"APR_INT64_T_FMT"\n", err_cnt);
            perf_cover_print_add_lines(cover_print_ctx, 1);
        }

        for (i = 0; i < g_thread_count; ++i) {
            if (!g_threads[i].endflag) {
                all_thread_end = 0;
                break;
            }
        }

        if (all_thread_end) {
            perf_cover_print_end(cover_print_ctx, 0);
            break;
        }

        perf_cover_print_end(cover_print_ctx, 1);

        apr_sleep(apr_time_from_sec(1));
    } while (1);

    for (i = 0; i < g_thread_count; i++) {
        perf_printf("thread%04d done : ok count(",
            g_threads[i].thread_no + 1);
        perf_green_printf("%"APR_INT64_T_FMT, g_threads[i].okcnt);
        perf_printf("), error count(");
        if (g_threads[i].errcnt > 0) {
            perf_red_printf("%"APR_INT64_T_FMT")\n", g_threads[i].errcnt);
        } else {
            perf_printf("%"APR_INT64_T_FMT")\n", g_threads[i].errcnt);
        }
    }

    return NULL;
}

// do performace test thread
static void* APR_THREAD_FUNC perf_worker(apr_thread_t *thread, void *data)
{ 
    thread_param_t  *param = (thread_param_t*)data;
    int              threadno = param->thread_no;
    perf_testcase_t *testcase = param->testcase;
    void            *testcase_ctx = param->testcase_ctx;
    apr_time_t       start = apr_time_now();

    if (g_duration_seconds > 0) {
        while (1) {
            apr_time_t now = apr_time_now();
            int ret;

            ret = testcase->do_perf(testcase_ctx, threadno);
            apr_thread_mutex_lock(param->mutex);
            if (ret == 0) {
                param->okcnt++;
            } else {
                param->errcnt++;
            }
            apr_thread_mutex_unlock(param->mutex);

            if (now - start >= apr_time_from_sec(g_duration_seconds)) {
                break;
            }
        }
    } else {
        apr_int64_t do_count;

        while (1) {
            int ret = testcase->do_perf(testcase_ctx, threadno);
            apr_thread_mutex_lock(param->mutex);
            if (ret == 0) {
                param->okcnt++;
            } else {
                param->errcnt++;
            }
            do_count = param->okcnt + param->errcnt;
            apr_thread_mutex_unlock(param->mutex);

            if (do_count >= g_loop_count) {
                break;
            }
        }
    }

    param->endflag = 1;

    return NULL;
}

int perf_add_test_case(perf_testcase_t *testcase)
{
    if (!g_pool) {
        apr_pool_initialize();
        apr_pool_create(&g_pool, NULL);
    }

    if (g_test_cases == NULL) {
        g_test_cases = apr_array_make(g_pool, 3, sizeof(perf_testcase_t*));
    }

    APR_ARRAY_PUSH(g_test_cases, perf_testcase_t*) = testcase;

    return 0;
}

static void perf_do(perf_testcase_t *testcase)
{
    apr_thread_t *h_print = NULL;
    apr_pool_t   *pool;
    void         *pctx = NULL;
    int           i, ret;

    if (testcase->do_setup) {
        ret = testcase->do_setup(g_thread_count, &pctx);
        if (ret != 0) {
            perf_red_printf("call perf test setup error!");
            return;
        }
    }

    apr_pool_create(&pool, g_pool);

    ret = apr_thread_create(&h_print, NULL, print_worker, testcase, pool);
    if (ret != APR_SUCCESS) {
        perf_red_printf("start print thread error!\n");
        return;
    }

    g_start_time = apr_time_now();
    for (i = 0; i < g_thread_count; ++i) {
        g_threads[i].thread_no = i;
        g_threads[i].testcase = testcase;
        g_threads[i].testcase_ctx = pctx;
        ret = apr_thread_mutex_create(&g_threads[i].mutex,
            APR_THREAD_MUTEX_DEFAULT, pool);
        if (ret != APR_SUCCESS) {
            perf_red_printf("create thread%04d thread mutex error!", i);
            continue;
        }
        ret = apr_thread_create(&g_threads[i].thread_handle,
            NULL, perf_worker, &g_threads[i], pool);
        if (ret != APR_SUCCESS) {
            perf_red_printf("start thread%04d error!\n", i);
        }
    }

    for (i = 0; i < g_thread_count; ++i) {
        apr_thread_join(&ret, g_threads[i].thread_handle);
    }

    apr_thread_join(&ret, h_print);

    if (testcase->do_teardown) {
        testcase->do_teardown(pctx);
    }

    apr_pool_destroy(pool);

    memset(g_threads, 0, sizeof(g_threads));
}

static void set_thread_count(const char *argv)
{
    g_thread_count = atoi(argv);
    if (g_thread_count < 1 || g_thread_count > 1024) {
        perf_yellow_printf("WARNING: "
            "threads must be between 1 - 1024, ignore it!\n");
        g_thread_count = 1;
    }
}

static void set_thread_loop_count(const char *argv)
{
    g_loop_count = atoi(argv);
    if (g_loop_count < 1) {
        perf_yellow_printf("WARNING: loop must be more than 1, ignore it!\n");
        g_loop_count = 1;
    }
}

static void set_duration_seconds(const char *argv)
{
    g_duration_seconds = atoi(argv);
    if (g_duration_seconds < 1) {
        perf_yellow_printf("WARNING: "
            "duration must be not less than 0, ignore it!\n");
        g_duration_seconds = 0;
    }
}

static int is_help(const char *argv)
{
    static const char *k_help_string[] = { "-h", "--h", "--help", "-help", NULL };
    const char **ptr = k_help_string;

    while (*ptr) {
        if (apr_strnatcasecmp(*ptr, argv) == 0) {
            return 1;
        }
        ptr++;
    }

    return 0;
}

static void usage(const char *program)
{
    printf("usage:\n");
    printf("  %s -h\n", program);
    printf("  %s case [threads=thread_count] [loop=loop_time_per_thread] "
        "[duration=duration_time]\n\n", program);

    printf("  -h:       show this help\n\n");
    printf("  threads:  the count of thread to start, default 1\n");
    printf("  loop:     the loop count per thread to test, defualt 1. It will be ignore if duration is set\n");
    printf("  duration: the duration of each test, in seconds, default 0\n\n");
    printf("  case:     the test case name, support the following:\n\n");

    if (g_test_cases) {
        int i;

        for (i = 0; i < g_test_cases->nelts; i++) {
            perf_testcase_t *testcase =
                APR_ARRAY_IDX(g_test_cases, i, perf_testcase_t*);
            if (testcase) {
                testcase->usage();
            }
        }
    }

    printf("\n\n");
}

static perf_testcase_t* perf_get_test_case(const char *name)
{
    int i;

    for (i = 0; i < g_test_cases->nelts; i++) {
        perf_testcase_t *tc = APR_ARRAY_IDX(g_test_cases, i, perf_testcase_t*);
        if (!tc) {
            continue;
        }
        if (strcmp(name, tc->name) == 0) {
            return tc;
        }
    }

    return NULL;
}

int perf_main(int argc, char *argv[])
{
    perf_testcase_t *testcase;
    char *case_argv[128] = { 0 };
    int case_argc = 0;
    int i, ret;

    if (argc < 2) {
        usage(argv[0]);
        return 0;
    }

    if (argc == 2 && is_help(argv[1])) {
        usage(argv[0]);
        return 0;
    }

    testcase = perf_get_test_case(argv[1]);
    if (testcase == NULL) {
        perf_red_printf("test case(%s) not support!\n\n", argv[1]);
        usage(argv[0]);
        return -1;
    }

    if (testcase->enviroment_init) {
        int ret = testcase->enviroment_init();
        if (ret != 0) {
            perf_red_printf("test case(%s) enviroment_init error!\n\n", argv[1]);
            return -2;
        }
    }

    for (i = 2; i < argc; ++i) {
        if (!strncmp(argv[i], "threads=", 8)) {
            set_thread_count(argv[i] + 8);
        } else if (!strncmp(argv[i], "loop=", 5)) {
            set_thread_loop_count(argv[i] + 5);
        } else if (!strncmp(argv[i], "duration=", 9)) {
            set_duration_seconds(argv[i] + 9);
        } else {
            case_argv[case_argc++] = argv[i];
        }
    }

    ret = testcase->parse_param(case_argc, case_argv);
    if (ret != 0) {
        perf_yellow_printf("WARNING: test case(%s) parse param error!");
        return -3;
    }

    perf_do(testcase);

    testcase->enviroment_terminate();

    apr_pool_terminate();

    return 0;
}

