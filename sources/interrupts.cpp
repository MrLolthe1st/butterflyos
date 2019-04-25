#include "includes/interrupts.h"

#define IDT_ADDRESS 0x9000
#define IDT_RANGE 0x9800
#define CODE_SEGMENT 0x08

/*
	Installs an interrupt with id to address with flags
*/
void install_interrupt_protected(unsigned char id, void * address, unsigned char flags) {

	char * it = (char*)IDT_ADDRESS;
	
	unsigned char a[8];

	a[0] = (unsigned int)address & 0x000000FF;
	a[1] = ((unsigned int)address & 0x0000FF00) >> 8;

	// Set it to Ring 0 CS
	a[2] = CODE_SEGMENT;
	a[3] = 0;
	a[4] = 0;

	a[5] = flags;
	a[6] = ((unsigned int)address & 0x00FF0000) >> 16;
	a[7] = ((unsigned int)address & 0xFF000000) >> 24;

	// And copy the copy to IDT
	for (int k = 0; k < 8; k++)
		* (it + id * 8 + k) = a[k];
}

/*
	Loads the IDT address to CPU
*/
void load_interrupts() {

	unsigned short * limit = (unsigned short*)IDT_RANGE;
	unsigned int * place = (unsigned int*)(IDT_RANGE + 2);
	*limit = 256 * 8 - 1;
	*place = IDT_ADDRESS;
	__asm__("lidt 0(,%0,)"::"a" (IDT_RANGE));

}


/* 
	There is some piece of bad code, do not beat me,
	it used for debugging, testing and other, in stable
	build it won't exists
*/

#include "includes/video.h"

long long time = 0;
IRQ_HANDLER(pit_timer)
{
	time++;
	time1024 += 1024 / 17;
	if (pressed_keys != 0 && (time - last_press) > 2)
	{
		keys_queue.insert(Key(pressed_keys, 1));
		last_press = time;
	}
}


IRQ_HANDLER(ps2_keyboard)
{
	keyboard_handle();
}

IDT_HANDLER(some)
{

}

IRQ_HANDLER(some1)
{
	//printf("&");
}

IRQ_HANDLER_PIC2(some2)
{
	//printf("~");
}

volatile long long time1024 = 0;

IRQ_HANDLER_PIC2(rtc_time)
{
	outportb(0x70, 0x0C);	// select register C
	inportb(0x71);			// just throw away contents
	time1024++;
}

void Wait(int cnt) {
	long long t = time1024; int a = 0;
	while ((time1024 - t) < cnt)
		// GCC tries to optimize it, just increment some integer
		a++;

}
void init_idt()
{
	// Remap PIC
	outportb(0x20, 0x11);
	outportb(0xa0, 0x11);

	// set first IRQs lines to 0x2#th IDT entries
	outportb(0x21, 0x20);

	// set second IRQ lines(8 - 15) to 0x2# IDT entries
	outportb(0xa1, 0x28);
	outportb(0x21, 0x04);
	outportb(0xa1, 0x02);
	outportb(0x21, 0x01);
	outportb(0xa1, 0x01);
	outportb(0x21, 0x00);
	outportb(0xa1, 0x00);

	// Fill the table
	for (int i = 0; i < 256; i++)
		install_interrupt_protected(i, &some, 0x8E);

	for (int i = 0x20; i < 0x30; i++)
		install_interrupt_protected(i, &some1, 0x8E);

	for (int i = 0x30; i < 0x40; i++)
		install_interrupt_protected(i, &some2, 0x8E);

	install_interrupt_protected(0x20, &pit_timer, 0x8E);

	install_interrupt_protected(0x21, &ps2_keyboard, 0x8E);

	install_interrupt_protected(0x28, &rtc_time, 0x8E);

	outportb(0x70, 0x8B);			// select register B, and disable NMI
	char prev = inportb(0x71);		// read the current value of register B
	outportb(0x70, 0x8B);			// set the index again (a read will reset the index to register D)
	outportb(0x71, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
	outportb(0x70, 0x0C);			// select register C
	inportb(0x71);					// just throw away contents
	load_interrupts();				// Load IDT to processor
	// And turn on interrupts
	__asm__("sti");
}