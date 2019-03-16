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

int main()
{

	clear_screen();
	init_memory();
	init_idt();
	storages_init();
	PciInit();
	printf("\n");

	map<int, int> m;
	long long t = time;
	for (int i = 0; i < 50; i++) {
		m[i] = i * 3;
	}
	map<int, int>::iterator it = m.begin();
	while (it != m.end()) {
		printf("[%d:%d]", it.first, it.second);
		it++;
	}
	for (;;) {
		Key z = keys_queue.get();
		if (z.keycode != 0 && z.pressed == 1)
		{
			print_char(z.keycode);
		}
	}
}