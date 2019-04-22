#include "includes/target.h"
#include "includes/string.h"
#include "includes/video.h"
#include "includes/memory/memory.h"
#include "includes/interrupts.h"
#include "includes/ports.h"
#include "includes/storage/storage.h"
#include "includes/PCI.h"
#include "std/trees.h"
#include "std/map.h"
#include "includes/storage/drives.h"
#include "usb/usbd.h"
int main()
{
	clear_screen();
	init_memory();
	init_idt();
	storages_init();
	PciInit();
	drives_init();
	printf("!");
	for (;;) {
		Key z = keys_queue.get();
		if (z.keycode != 0 && z.pressed == 1)
			print_char(z.keycode);
		UsbPoll();
		//__asm__("hlt");
	}
}