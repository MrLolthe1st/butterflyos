#ifndef _VIDEO_H
#define _VIDEO_H (1)
#define WIDTH 80
#define HEIGHT 25
#define VIDEO_SEG 0xB8000
#include <stdarg.h>
#include "string.h"
void scroll();
void clear_screen();
int print_string(const char * s);
int print_char(char c);
int print_int(int a, int s);
int print_int(int a, int s, int cnt);
#endif