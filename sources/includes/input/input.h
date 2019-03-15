#ifndef _INPUT_H
#define _INPUT_H (1)
#include "ps2.h"
#define QUEUE_MAX 512
namespace input {
	class Key {
	public:
		short keycode = 0;
		char pressed = 0;
		Key(short z, char a) { keycode = z; pressed = a; };
		Key() {};
	};
	class KeyQueue {
		Key queue[QUEUE_MAX];
		int length = 0, head = 0, tail = -1;
	public:
		KeyQueue() { };
		void insert(Key k) {
			if (length == QUEUE_MAX)
			{
				//Queue overflow
				return;
			}
			tail = (tail + 1) % QUEUE_MAX;
			queue[tail] = k;
			length++;
		};
		input::Key get()
		{
			if (length == 0) return Key(0, 0);
			Key z = queue[head];
			head = (head + 1) % QUEUE_MAX;
			length--;
			return z;
		};
	};
}
using namespace input;
extern KeyQueue keys_queue;
extern long long last_press;
extern char pressed_keys;
#endif