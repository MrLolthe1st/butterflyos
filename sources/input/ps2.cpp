#include "../includes/input/ps2.h"
#include "../includes/video.h"
int shift = 0;
char codes[] = { 0,0,'1','2','3','4','5','6','7','8','9','0','-','=',8,'\t','q','w','e','r','t','y','u','i','o','p','[',']',13,0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,'+',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
char codes_sh[] = { 0,0,'!','@','#','$','%','^','&','*','(',')','_','+',8,'\t','Q','W','E','R','T','Y','U','I','O','P','{','}',13,0,'A','S','D','F','G','H','J','K','L',':','"','~',0,'|','Z','X','C','V','B','N','M','<','>','?',0,'*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,'+',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

int ls = 0;
void keyboard_handle()
{
	unsigned char o = inportb(0x60);

	if (o == 0x36 || o == 0x2A)
		shift = 1; //Shift нажали
	else if (o == 0x36 + 0x80 || o == 0x2A + 0x80)
		shift = 0; //Shift отпустили
	if (shift) {

		if (o < 0x80) {
			ls = o;
			pressed_keys = codes_sh[o & 0x7F];
			last_press = time + 2;
		}
		else if (ls == (o & 0x7F)) {
			ls = 0;
			pressed_keys = 0;
		}
		keys_queue.insert(Key(codes_sh[o & 0x7F], o < 0x80));
	}
	else {
		keys_queue.insert(Key(codes[o & 0x7F], o < 0x80));
		if (o < 0x80) {
			ls = o;
			pressed_keys = codes[o & 0x7F];
			last_press = time + 2;
		}
		else if (ls == (o & 0x7F)) {
			ls = 0;
			pressed_keys = 0;
		}
	}/*
	print_int(o, 16, 2);
	print_int(ls, 16, 2);
	print_int(pressed_keys, 16, 2);
	print_string(" ");*/
	o = inportb(0x61); //отправим контроллеру клавы сигнал о том, что мы завершили работу с клавой
	o |= 1;
	outportb(0x61, o);
}