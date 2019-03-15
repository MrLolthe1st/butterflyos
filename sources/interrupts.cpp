#include "includes/interrupts.h"
#define IDT_ADDRESS 0x9000
#define IDT_RANGE 0x9800
#define CODE_SEGMENT 0x08
void inst(unsigned char interruptID, void * address, unsigned char flags) {
	char * it = (char*)IDT_ADDRESS;
	unsigned char i;
	unsigned char a[8];
	a[0] = (unsigned int)address & 0x000000FF;
	a[1] = ((unsigned int)address & 0x0000FF00) >> 8;
	a[2] = CODE_SEGMENT;
	a[3] = 0;
	a[4] = 0;
	a[5] = flags;
	a[6] = ((unsigned int)address & 0x00FF0000) >> 16;
	a[7] = ((unsigned int)address & 0xFF000000) >> 24;
	for (int k = 0; k < 8; k++)
		* (it + interruptID * 8 + k) = a[k];
}

// ------------------------------------------------------------------------------------------------
void int_l() {
	unsigned short * limit = (unsigned short*)IDT_RANGE;
	unsigned int * place = (unsigned int*)(IDT_RANGE + 2); *limit = 256 * 8 - 1; *place = IDT_ADDRESS;
	__asm__("lidt 0(,%0,)"::"a" (IDT_RANGE));
}

#include "includes/video.h"
long long time = 0;
IRQ_HANDLER(pit_timer)
{
	time++;
	if (pressed_keys != 0 && (time - last_press) > 1)
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

}

IRQ_HANDLER_PIC2(some2)
{

}

void init_idt()
{
	outportb(0x20, 0x11);
	outportb(0xa0, 0x11);
	outportb(0x21, 0x20);
	outportb(0xa1, 0x28);
	outportb(0x21, 0x04);
	outportb(0xa1, 0x02);
	outportb(0x21, 0x01);
	outportb(0xa1, 0x01);
	outportb(0x21, 0x00);
	outportb(0xa1, 0x00);
	for (int i = 0; i < 256; i++)
		inst(i, &some, 0x8E);
	for (int i = 0x20; i < 0x30; i++)
		inst(i, &some1, 0x8E);
	for (int i = 0x30; i < 0x40; i++)
		inst(i, &some2, 0x8E);
	inst(0x20, &pit_timer, 0x8E);
	inst(0x21, &ps2_keyboard, 0x8E);
	int_l();
	__asm__("sti");
}