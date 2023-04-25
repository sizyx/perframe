/*
 * File: perframe_printf.c
 * Author: sizyx@163.com
 * Description:
 *   this is the source file of colored printf
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "perframe.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

void perf_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void perf_red_printf(const char *fmt, ...)
{
#ifdef WIN32
    const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO buffer_info;
    WORD old_color_attrs;

    va_list args;
    va_start(args, fmt);
    // Gets the current text color.
    
    GetConsoleScreenBufferInfo(stdout_handle, &buffer_info);
    old_color_attrs = buffer_info.wAttributes;
    // We need to flush the stream buffers into the console before each
    // SetConsoleTextAttribute call lest it affect the text that is already
    // printed but has not yet reached the console.
    fflush(stdout);
    SetConsoleTextAttribute(stdout_handle,
        FOREGROUND_RED | FOREGROUND_INTENSITY);
    vprintf(fmt, args);

    fflush(stdout);
    // Restores the text color.
    SetConsoleTextAttribute(stdout_handle, old_color_attrs);
    va_end(args);
#else
    va_list args;
    va_start(args, fmt);

    printf("\033[0;3%sm", "1");
    vprintf(fmt, args);
    printf("\033[m");  // Resets the terminal to default.

    va_end(args);
#endif

    //printf("\n");
}

void perf_yellow_printf(const char *fmt, ...)
{
#ifdef WIN32
    const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO buffer_info;
    WORD old_color_attrs;

    va_list args;
    va_start(args, fmt);
    // Gets the current text color.

    GetConsoleScreenBufferInfo(stdout_handle, &buffer_info);
    old_color_attrs = buffer_info.wAttributes;
    // We need to flush the stream buffers into the console before each
    // SetConsoleTextAttribute call lest it affect the text that is already
    // printed but has not yet reached the console.
    fflush(stdout);
    SetConsoleTextAttribute(stdout_handle,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    vprintf(fmt, args);

    fflush(stdout);
    // Restores the text color.
    SetConsoleTextAttribute(stdout_handle, old_color_attrs);
    va_end(args);
#else
    va_list args;
    va_start(args, fmt);

    printf("\033[0;3%sm", "3");
    vprintf(fmt, args);
    printf("\033[m");  // Resets the terminal to default.

    va_end(args);
#endif

    //printf("\n");
}

void perf_green_printf(const char *fmt, ...)
{
#ifdef WIN32
    const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO buffer_info;
    WORD old_color_attrs;

    va_list args;
    va_start(args, fmt);
    // Gets the current text color.

    GetConsoleScreenBufferInfo(stdout_handle, &buffer_info);
    old_color_attrs = buffer_info.wAttributes;
    // We need to flush the stream buffers into the console before each
    // SetConsoleTextAttribute call lest it affect the text that is already
    // printed but has not yet reached the console.
    fflush(stdout);
    SetConsoleTextAttribute(stdout_handle,
        FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    vprintf(fmt, args);

    fflush(stdout);
    // Restores the text color.
    SetConsoleTextAttribute(stdout_handle, old_color_attrs);
    va_end(args);
#else
    va_list args;
    va_start(args, fmt);

    printf("\033[0;3%sm", "2");
    vprintf(fmt, args);
    printf("\033[m");  // Resets the terminal to default.

    va_end(args);
#endif

    //printf("\n");
}

void perf_printhex(const char *prefix,
    const unsigned char *data, unsigned int len)
{
    unsigned int i;

    perf_yellow_printf("%s Length(%d):\n", prefix, len);
    for (i = 0; i < len; i++) {
        printf("%02X ", *(data + i));
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    if (len % 16 != 0) {
        printf("\n");
    }

    return;
}

typedef struct cover_print_ctx {
#ifdef WIN32
    HANDLE hStdout;
    COORD  startpos;
#else
    int    line_count;
#endif
} cover_print_ctx;

void* perf_cover_print_start()
{
#ifdef WIN32
    CONSOLE_SCREEN_BUFFER_INFO buffer_info;
#endif
    cover_print_ctx *pctx = calloc(1, sizeof(cover_print_ctx));
    if (pctx == NULL) {
        return NULL;
    }
#ifdef WIN32
    pctx->hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(pctx->hStdout, &buffer_info);
    pctx->startpos.X = buffer_info.dwCursorPosition.X;
    pctx->startpos.Y = buffer_info.dwCursorPosition.Y;
#endif

    return pctx;
}

void perf_cover_print_add_lines(void *pctx, int line)
{
    cover_print_ctx *ctx = (cover_print_ctx*)pctx;
    if (!ctx) {
        return;
    }
#ifdef WIN32

#else
    ctx->line_count += line;
#endif

    return;
}

void perf_cover_print_end(void *pctx, int gotostart)
{
    cover_print_ctx *ctx = (cover_print_ctx*)pctx;

    if (!ctx) {
        return;
    }

    if (gotostart) {
#ifdef WIN32
        SetConsoleCursorPosition(ctx->hStdout, ctx->startpos);
#else
        int i;
        for (i = 0; i <= ctx->line_count; ++i) {
            printf("\r\b");
        }
#endif
    }

    free(ctx);

    return;
}
