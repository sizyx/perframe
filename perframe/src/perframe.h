/*
 * File: perframe.h
 * Author: sizyx@163.com
 * Description:
 *   this is the header file of performance test frame
 */

#ifndef _PERF_FRAME_H_
#define _PERF_FRAME_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct perf_testcase_s perf_testcase_t;   // test case struct

typedef enum {
    e_speed_count      = 1,    // calculate the speed according to the counts per second
    e_speed_throughput = 2,    //Calculate speed by throughput per second
} perf_speed_e;

struct perf_testcase_s {
    const char  *name;
    perf_speed_e stype;

    unsigned int block_size;

    void (*usage)(void);

    int (*enviroment_init)(void);
    int (*enviroment_terminate)(void);

    int (*parse_param)(int argc, char *argv[]);
    int (*do_setup)(int thread_count, void **pctx);
    int (*do_perf)(void *pctx, int thread_idx);
    int (*do_teardown)(void *pctx);
};

void perf_printf(const char *fmt, ...);

void perf_red_printf(const char *fmt, ...);

void perf_yellow_printf(const char *fmt, ...);

void perf_green_printf(const char *fmt, ...);

void perf_printhex(const char *prefix,
    const unsigned char *data, unsigned int len);

void* perf_cover_print_start();

void perf_cover_print_add_lines(void *pctx, int line_count);

void perf_cover_print_end(void *pctx, int gotostart);

int perf_get_input_int(int default_val, int min_val, int max_val);

size_t perf_get_input_str(size_t n, char *buff);

void perf_clear_screen(void);


int perf_add_test_case(perf_testcase_t *test_case);

int perf_main(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif /* _PERF_FRAME_H_ */
