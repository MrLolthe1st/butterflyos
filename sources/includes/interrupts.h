#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H (1)
#include "ports.h"
#define IDT_HANDLER(func) unsigned char func = 0x90;\
__asm__(#func ": \n call __"#func " \n iret \n");\
extern "C" void _## func()
#define IRQ_HANDLER(func) unsigned char func = 0x90;\
__asm__(#func ": \npusha \n call __"#func " \n movb $0x20, %al \n outb %al, $0x20 \n popa  \n iret \n");\
extern "C" void _## func()
#define IRQ_HANDLER_PIC2(func) unsigned char func = 0x90;\
__asm__(#func ": \npusha \n call __"#func " \n movb $0x20, %al \n outb %al, $0x20 \n movb $0x20, %al \n outb %al, $0xA0 \n popa  \n iret \n");\
extern "C" void _## func()
void init_idt();
void inst(unsigned char interruptID, void * address, unsigned char flags);
#endif