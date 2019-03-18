#include "../includes/memory/memory.h"

void * heap_pool = (void*)HEAP_START;
size_t heap_size = 0;
size_t count_memory()
{

	unsigned int * mem = (unsigned int *)HEAP_START;
	int count = 0;
	do {
		unsigned int val = *mem;
		*mem = 0xBE0DEAD;
		unsigned int val1 = *mem;
		*mem = val;
		if (!(val1 == 0xBE0DEAD))
			break;
		count++;
		mem = (unsigned int*)((size_t)mem + 1024);
	} while (1);
	return count * 1024;
}

void init_memory()
{
	heap_size = count_memory();
	//print_int(heap_size / 1024, 10);
	//print_string("KB heap ready.\n");
	init_memory_pool(heap_size, heap_pool);
}

void * malloc(size_t size)
{
	return malloc_ex(size, heap_pool);
}

void * realloc(void * a, size_t b)
{
	return realloc_ex(a, b, heap_pool);
}

void free(void *ptr)
{
	free_ex(ptr, heap_pool);
}
void * operator new(size_t size)
{
	return malloc(size);
}
void operator delete(void * ptr)
{
	return free(ptr);
}
