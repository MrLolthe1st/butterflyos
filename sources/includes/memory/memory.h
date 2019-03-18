#ifndef _MEMORY_H
#define _MEMORY_H (1)
#include "../target.h"
//#include "../video.h"
#include "../../memory/tlsf.h"
size_t count_memory();
void init_memory();
void *malloc(size_t size);
void free(void *ptr);
void * malloc(size_t size);
void * operator new(size_t size);
void operator delete(void * ptr);

#endif