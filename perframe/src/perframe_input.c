/*
 * File: perframe_input.c
 * Author: sizyx@163.com
 * Description:
 *   this is the source file of input
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "perframe.h"

#if defined(WIN32) || defined(_WIN64)
#include <Windows.h>
#include <conio.h>
#define PERF_ENTER     0x0D
#define PERF_BACKSPACE 0x08
#else
#include <dlfcn.h>
#include <termios.h>
#include <unistd.h>
#define PERF_ENTER     0x0A
#define PERF_BACKSPACE 0x7F
static int _getch()
{
    int ch = 0;
    struct termios new_setting, init_setting;

    //get termios setting and save it 
    tcgetattr(0, &init_setting);
    new_setting = init_setting;
    new_setting.c_lflag &= IGNCR;

    tcsetattr(0, TCSANOW, &new_setting);
    ch = getchar();

    //restore the setting
    tcsetattr(0, TCSANOW, &init_setting);
    //putchar(ch);
    return ch;
}
#endif

int perf_get_input_int(int default_val, int min_val, int max_val)
{
    char buf[32] = {0};
    int ch;
    unsigned int offset = 0;

    while (offset < sizeof(buf)) {
        ch = _getch();
        switch(ch) {
        case PERF_ENTER:
            if (strlen(buf) == 0) {
                perf_green_printf("%d\n", default_val);
                return default_val;
            }
            if (atoi(buf) >= min_val) {
                printf("\n");
                return atoi(buf);
            }
            break;
        case PERF_BACKSPACE:
            if (offset > 0) {
                printf("\b \b");
                offset -= 1;
                buf[offset] = '\0';
            }
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            buf[offset++] = ch;
            buf[offset] = '\0';

            if (atoi(buf) <= max_val) {
                perf_green_printf("%c", ch);
            } else {
                offset -= 1;
                buf[offset] = '\0';
            }
            break;
        default:
            break;
        }
    }

    return atoi(buf);
}

size_t perf_get_input_str(size_t n, char *buff)
{
    size_t offset = 0;
    int ch;

    memset(buff, 0, n);

    while (offset < n - 1) {
        ch = _getch();
        switch(ch) {
        case PERF_ENTER:
            printf("\n");
            return strlen(buff);
        case PERF_BACKSPACE:
            if (offset > 0) {
                printf("\b \b");
                offset -= 1;
                buff[offset] = '\0';
            }
            break;
        default:
            if (ch >= 32 && ch < 127) {
                buff[offset++] = ch;
                buff[offset] = '\0';
                perf_green_printf("%c", ch);
            }
            break;
        }
    }

    return strlen(buff);
}

void perf_clear_screen()
{
#ifdef WIN32
    system("cls");
#else
    system("clear");
#endif
}
