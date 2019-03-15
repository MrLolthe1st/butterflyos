#include "includes/string.h"
void *memcpy(void *dest, const void *source, size_t count)
{
	char * c = (char*) dest;
	const char * s = (const char*) source;
	for(size_t i = 0; i < count; i++)
		*c++ = *s++;
}
void *memset(void *buf, int ch, size_t count)
{
	char * c = (char*) buf;
	for(size_t i = 0; i < count; i++)
		*c++ = ch;
}