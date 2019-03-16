#ifndef _STRING_H
#define _STRING_H (1)
#include "target.h"
#include <stdarg.h>
void *memcpy(void *dest, const void *source, size_t count);
void *memset(void *buf, int ch, size_t count);
char * strposn(const char *p, const char *q);
char * strpbrk(const char *s1, const char *s2);
int strpat(const char *s1, const char *s2);
int strnwrd(const char *p);
char * strncpy(char *dst, const char *src, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
char * strnchr(const char *p, char c, size_t n);
char * strncat(char *dst, const char *src, size_t n);
void strmerge(char *d, const char *s);
size_t strlen(const char *p);
size_t strlcpy(char *d, const char *s, size_t l);
char * strcpy(char *d, const char *s);
int strcmp(const char *s1, const char *s2);
char * strcat(char *dst, const char *src);
char * strccat(char *dst, int c);
char * strchr(const char *p, int c);
char * strichr(char *p, int c);
int gethex(int *vp, char *p, int n);
//char * llbtoa(char *dst, int value, int base);
int printf(const char * str, ...);
#endif