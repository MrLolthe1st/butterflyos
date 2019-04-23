#include <stdio.h>
int main()
{
	FILE * a = fopen("bootloader", "rb");
	FILE * b = fopen("disk.img", "rb");
	FILE * c = fopen("c.img", "wb");
	fseek(b, 0, 2);
	int fsize = ftell(b);
	rewind(b);
	for (int i = 0; i < 0x5A; i++)
	{
		char cc;
		fread(&cc, 1, 1, b);
		fwrite(&cc, 1, 1, c);
		fread(&cc, 1, 1, a);
	}
	for (int i = 0x5A; i <= 0x1FF; i++)
	{
		char cc;
		fread(&cc, 1, 1, b);
		fread(&cc, 1, 1, a);
		fwrite(&cc, 1, 1, c);
	}
	for (int i = 2; i < fsize >> 9; i++)
	{
		char cc[512];
		fread(&cc, 512, 1, b);
		fwrite(&cc, 512, 1, c);

	}
	return 0;
}