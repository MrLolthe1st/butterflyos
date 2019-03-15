#include "includes/ports.h"
// ------------------------------------------------------------------------------------------------
static inline void MmioWrite8(void *p, u8 data)
{
	*(volatile u8 *)(p) = data;
}


// ------------------------------------------------------------------------------------------------
static inline u8 MmioRead8(void *p)
{
	return *(volatile u8 *)(p);
}


// ------------------------------------------------------------------------------------------------
static inline void MmioWrite16(void *p, u16 data)
{
	*(volatile u16 *)(p) = data;
}


// ------------------------------------------------------------------------------------------------
static inline u16 MmioRead16(void *p)
{
	return *(volatile u16 *)(p);
}


// ------------------------------------------------------------------------------------------------
static inline void MmioWrite32(void *p, u32 data)
{
	*(volatile u32 *)(p) = data;
}


// ------------------------------------------------------------------------------------------------
static inline u32 MmioRead32(void *p)
{
	return *(volatile u32 *)(p);
}


// ------------------------------------------------------------------------------------------------
static inline void MmioWrite64(void *p, u64 data)
{
	*(volatile u64 *)(p) = data;
}


// ------------------------------------------------------------------------------------------------
static inline u64 MmioRead64(void *p)
{
	return *(volatile u64 *)(p);
}


// ------------------------------------------------------------------------------------------------
static inline void MmioReadN(void *dst, const volatile void *src, size_t bytes)
{
	volatile u8 *s = (volatile u8 *)src;
	u8 *d = (u8 *)dst;
	while (bytes > 0)
	{
		*d = *s;
		++s;
		++d;
		--bytes;
	}
}

// ------------------------------------------------------------------------------------------------
uint8_t inportb(uint16_t portid)
{
	uint8_t ret;
	__asm__("inb %%dx, %%al":"=a"(ret) : "d"(portid));
	return ret;
}

// ------------------------------------------------------------------------------------------------
uint16_t inportw(uint16_t portid)
{
	uint16_t ret;
	__asm__("inw %%dx, %%ax":"=a"(ret) : "d"(portid));
	return ret;
}

// ------------------------------------------------------------------------------------------------
uint32_t inportd(uint16_t portid)
{
	uint32_t ret;
	__asm__("inl %%dx, %%eax":"=a"(ret) : "d"(portid));
	return ret;
}

// ------------------------------------------------------------------------------------------------
void outportb(uint16_t portid, uint8_t value)
{
	__asm__("outb %%al, %%dx": : "d" (portid), "a" (value));
}

// ------------------------------------------------------------------------------------------------
void outportw(uint16_t portid, uint16_t value)
{
	__asm__("outw %%ax, %%dx": : "d" (portid), "a" (value));
}

// ------------------------------------------------------------------------------------------------
void outportd(uint16_t portid, uint32_t value)
{
	__asm__("outl %%eax, %%dx": : "d" (portid), "a" (value));
}
