#ifndef _TARGET_H
#define _TARGET_H (1)
#define size_t unsigned int
#ifndef uintptr_t
#define uintptr_t size_t
#endif
#ifndef NULL
#define NULL 0
#endif
#define HEAP_START 0x200000
#endif