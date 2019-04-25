#include "includes/video.h"
int cursor = 0, attr = 0x0F;

void *memcpy(void *dest, const void *source, size_t count);
void *memset(void *buf, int ch, size_t count);
void scroll()
{
	memcpy((char*)VIDEO_SEG, (char*)VIDEO_SEG + WIDTH * 2, (HEIGHT - 1) * WIDTH * 2);
	char * mem = (char*)VIDEO_SEG + (HEIGHT - 1) * WIDTH * 2;
	for (int i = (HEIGHT - 1) * WIDTH * 2; i < HEIGHT * WIDTH * 2; i++)
		*mem++ = 0;
}

void clear_screen()
{
	cursor = 0;
	char * mem = (char*)VIDEO_SEG;
	for (int i = 0; i < WIDTH * HEIGHT; i++)
		*mem++ = ' ', *mem++ = 0;
}

void set_attr(int att) {
	attr = att;
}

int get_attr() {
	return attr;
}

int print_char(char c)
{
	if (c == '\n')
	{
		int ox = cursor / WIDTH;
		ox++;
		if (ox == HEIGHT) scroll(), ox--;
		cursor = ox * WIDTH;
		return 1;
	}
	if ((cursor / WIDTH) >= HEIGHT) scroll(), cursor -= WIDTH;
	if (c == 13)
		return 0;
	char * mem = (char*)VIDEO_SEG + cursor * 2;
	*mem++ = c;
	*mem++ = attr;
	cursor++;
    return 1;
}

int print_string(const char * s)
{
	if (s) while (*s) print_char(*s++);
	else return 0;
	return 1;
}

const char * table = "0123456789ABCDEF";
int print_int(int a, int s)
{
	if (a < 0) print_char('-'), a = -a;
	char res[32];
	int idx = 31;
	res[idx] = '0';
	while (a > 0)
	{
		res[idx--] = table[a % s];
		a /= s;
	}
	if (idx < 31) idx++;
	for (; idx < 32; idx++) print_char(res[idx]);
	return 1;
}

int print_int(int a, int s, int cnt)
{
	if (a < 0) print_char('-'), a = -a;
	char res[32];
	int idx = 31;
	for (int i = 0; i < 32; i++) res[i] = '0';
	while (a > 0)
	{
		res[idx--] = table[a % s];
		a /= s;
	}
	if (idx < 31) idx++;
	for (idx = 32 - cnt; idx < 32; idx++) print_char(res[idx]);
	return 1;
}

int get_debug_color(int el)
{
	switch (el)
	{
	case DEBUG_OK:
		return 0x0a;
	case DEBUG_ERROR:
		return 0x0c;
	case DEBUG_WARNING:
		return 0x0e;
	case DEBUG_INFO:
		return 0x0b;
	default:
		break;
	}
	return 0x0F;
}

char * get_debug_label(int el)
{
	switch (el)
	{
	case DEBUG_OK:
		return (char*) "OK";
	case DEBUG_ERROR:
		return (char*) "ERROR";
	case DEBUG_WARNING:
		return (char*) "WARNING";
	case DEBUG_INFO:
		return (char*) "INFO";
	default:
		break;
	}
}
int log_debug(int error_level, const char * str, ...)
{
	if (!str)
		return 0;
	if (cursor % WIDTH) printf("\n");

	char buffer[1024];
	print_char('[');
	int at = attr;
	attr = get_debug_color(error_level);
	printf(" %7s ", get_debug_label(error_level));
	attr = at;
	print_char(']');
	print_char(' ');
	va_list ap;
	va_start(ap, str);

	int res = vsprintf(buffer, (char *)str, ap);
	print_string(buffer);
	printf("\n");
	return res;
}