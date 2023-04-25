/*
 * File: perf_benchmark.c
 * Author: sizyx@163.com
 * Description:
 *   this is the source file of bechmark sample
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#if !defined(WIN32) && !defined(_WIN64)
#include <alloca.h>
#else
#define alloca _alloca
#endif
#include "perframe.h"

static void bench_usage(void);
static void bench_usage2(void);
static int bench_parse_param(int argc, char *argv[]);
static int bench_enviroment_init(void);
static int bench_enviroment_terminate(void);
static int bench_setup(int thread_count, void **ppctx);
static int bench_perf_do(void *pctx, int threadno);
static int bench_teardown(void *pctx);


static perf_testcase_t k_benchmark = {
    "benchmark",                /* name */
    e_speed_count,              /* stype */
    1024,                       /* block_size */
    bench_usage,                /* usage */
    bench_enviroment_init,      /* enviroment_init */
    bench_enviroment_terminate, /* enviroment_terminate */
    bench_parse_param,          /* parse_param */
    bench_setup,                /* do_setup */
    bench_perf_do,              /* do_perf */
    bench_teardown              /* do_teardown */
};

static perf_testcase_t k_benchmark2 = {
    "benchmark2",               /* name */
    e_speed_throughput,         /* stype */
    8192,                       /* block_size */
    bench_usage2,               /* usage */
    bench_enviroment_init,      /* enviroment_init */
    bench_enviroment_terminate, /* enviroment_terminate */
    bench_parse_param,          /* parse_param */
    bench_setup,                /* do_setup */
    bench_perf_do,              /* do_perf */
    bench_teardown              /* do_teardown */
};

static void bench_usage(void)
{
    printf("    benchmark [block=block_size] \n");
    printf("      block: the size of block\n\n");
}

static void bench_usage2(void)
{
    printf("    benchmark2 block=[block_size] \n");
    printf("      block:            the size of block\n\n");
}

static int bench_parse_param(int argc, char *argv[])
{
    int i;

    for (i = 0; i < argc; i++) {
        int block_size;

        printf("bench_main: argv[%d]:%s\n", i, argv[i]);
        if (!strncmp(argv[i], "block=", 6)) {
            block_size = atoi(argv[i] + 6);
            if (block_size < 1 || block_size % 16 != 0) {
                perf_yellow_printf("WARNING: "
                    "block size must be a multiple of 16, use defualt 1024!\n");
                block_size = 1024;
            }
            k_benchmark.block_size = block_size;
            k_benchmark2.block_size = block_size;
        }
    }

    return 0;
}

static int bench_enviroment_init(void)
{
    // do some enviroment initalize
    perf_green_printf("bench_enviroment_init ok...\n");

    return 0;
}

static int bench_enviroment_terminate(void)
{
    // do some enviroment finalize
    perf_green_printf("bench_enviroment_terminate ok...\n");

    return 0;
}

static int bench_setup(int thread_count, void **ppctx)
{
    int *ary, i;

    if (thread_count < 1) {
        *ppctx = NULL;
        return -1;
    }

    ary = calloc(1, sizeof(int) * thread_count);
    if (ary == NULL) {
        return -2;
    }

    for (i = 0; i < thread_count; i++) {
        ary[i] = i + 1;
    }

    *ppctx = ary;

    srand((unsigned int)time(NULL));

    return 0;
}

static int bench_perf_do(void *pctx, int threadno)
{
    int *ary = (int*)pctx;

    if (!ary) {
        return -1;
    }

    if (ary[threadno] != threadno + 1) {
        return -2;
    }

    alloca(k_benchmark.block_size);

    if (rand() % 2 == 0) {
        return -3;
    }

    return 0;
}

static int bench_teardown(void *pctx)
{
    int *ary = (int*)pctx;

    if (!ary) {
        return -1;
    }

    free(ary);

    return 0;
}

int main(int argc, char *argv[])
{
    int ret;

    ret = perf_add_test_case(&k_benchmark);
    if (ret != 0) {
        perf_red_printf("perf_add_test_case error!\n");
        return ret;
    }

    ret = perf_add_test_case(&k_benchmark2);
    if (ret != 0) {
        perf_red_printf("perf_add_test_case error!\n");
        return ret;
    }

    return perf_main(argc, argv);
}

