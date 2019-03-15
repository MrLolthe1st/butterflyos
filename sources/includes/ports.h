#ifndef _PORTS_H
#define _PORTS_H (1)
#include "target.h"
#define uint16_t unsigned short
#define uint32_t unsigned int
#define uint8_t unsigned char
#define uint64_t unsigned long long
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
void outportd(uint16_t portid, uint32_t value);
void outportw(uint16_t portid, uint16_t value);
void outportb(uint16_t portid, uint8_t value);
uint32_t inportd(uint16_t portid);
uint16_t inportw(uint16_t portid);
uint8_t inportb(uint16_t portid);
#endif