#include "includes/target.h"
#include "includes/string.h"
#include "includes/video.h"
#include "includes/memory/memory.h"
#include "includes/interrupts.h"
#include "includes/ports.h"
#include "includes/storage/storage.h"

int main()
{
	clear_screen();
	init_memory();
	init_idt();
	storages_init();
	for(;;);
}