#include "includes/video.h"
int cursor = 0, attr = 0x0F;

void *memcpy(void *dest, const void *source, size_t count);
void *memset(void *buf, int ch, size_t count);
void scroll()
{
	memcpy((char*) VIDEO_SEG, (char*) VIDEO_SEG + WIDTH * 2, (HEIGHT - 1) * WIDTH * 2);
	char * mem = (char*) VIDEO_SEG + (HEIGHT - 1) * WIDTH * 2;
	for(int i = (HEIGHT - 1) * WIDTH * 2; i < HEIGHT * WIDTH * 2; i++)
		*mem++ = 0;
}

void clear_screen()
{
	cursor = 0;
	char * mem = (char*) VIDEO_SEG;
	for(int i = 0; i < WIDTH * HEIGHT; i++)
		*mem++ = ' ', *mem++ = 0;
}

int print_char(char c)
{
	if (c == '\n')
	{
		int ox = cursor / WIDTH;
		ox++;
		if(ox == HEIGHT) scroll(), ox--;
		cursor = ox * WIDTH;
		return 1;
	}
	if((cursor / WIDTH) >= HEIGHT) scroll(), cursor -= WIDTH;
	if(c == 13)
		return 0;
	char * mem = (char*) VIDEO_SEG + cursor * 2;
	*mem++ = c;
	*mem++ = attr;
	cursor++;
}

int print_string(const char * s)
{
	if(s) while(*s) print_char(*s++);
	else return 0;
	return 1;
}

const char * table = "0123456789ABCDEF";
int print_int(int a, int s)
{
	if(a < 0) print_char('-'), a = -a;
	char res[32];
	int idx = 31;
	res[idx] = '0';
	while(a > 0)
	{
		res[idx--] = table[a % s];
		a /= s;
	}
	if(idx < 31) idx++;
	for(; idx < 32; idx++) print_char(res[idx]);
	return 1;
}

int print_int(int a, int s, int cnt)
{
	if(a < 0) print_char('-'), a = -a;
	char res[32];
	int idx = 31;
	for(int i = 0; i < 32; i++) res[i] = '0';
	while(a > 0)
	{
		res[idx--] = table[a % s];
		a /= s;
	}
	if(idx < 31) idx++;
	for(idx = 32 - cnt; idx < 32; idx++) print_char(res[idx]);
	return 1;
}
