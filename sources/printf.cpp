
#pragma GCC push_options
#pragma GCC optimize ("Ofast")
#include <stdarg.h>
char tbuf[32];
char bchars[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

void __itoa(unsigned i, unsigned base, char* buf) {
	int pos = 0;
	int opos = 0;
	int top = 0;

	if (i == 0 || base > 16) {
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

	while (i != 0) {
		tbuf[pos] = bchars[i % base];
		pos++;
		i /= base;
	}
	top = pos--;
	for (opos = 0; opos < top; pos--, opos++)
		buf[opos] = tbuf[pos];
	buf[opos] = 0;
}

void __itoa_s(int i, unsigned base, char* buf) {
	if (base > 16) return;
	if (i < 0) {
		*buf++ = '-';
		i *= -1;
	}
	__itoa(i, base, buf);
}
int __qkprintf(const char* str, ...) {
	if (!str)
		return 0;
	va_list ap;
	va_start(ap, str);
	__kprintf_va_list((char *)str, ap);
	return 1;
}
void _kprintf(const char* str, ...) {
	if (!str)
		return;
	va_list ap;
	va_start(ap, str);
	__kprintf_va_list((char *)str, ap);
	return;
}
int kprintf(const char* str, ...) {
	if (!str)
		return 0;
	va_list ap;
	va_start(ap, str);
	__kprintf_va_list((char *)str, ap);
	return 1;
}
//Gets formatted string length
int formattedLength(char* str, va_list ap)
{
	int a = 0;
	char * s = 0;
	for (size_t i = 0; i < 4096; i++)
	{
		if (str[i] == 0) break;
		if (str[i] == '%')
		{
			switch (str[i + 1])
			{
				/** string **/
			case 's':
				s = va_arg(ap, char*);
				while (*s != 0) { a++; s++; }
				i++;
				continue;
				/** decimal **/
			case 'd': {
				int c = va_arg(ap, int);
				char str[32] = { 0 };
				__itoa_s(c, 10, str);
				char * s = (char*)&str;
				while (*s != 0) { a++; s++; };
				i++;
				continue;
			}
			case 'x': {
				int c = va_arg(ap, int);
				char str[32] = { 0 };
				__itoa(c, 16, str);
				char * s = (char*)&str;
				while (*s != 0) { a++; s++; };
				i++;
				continue;
			}
					  /** character **/
			case 'c': {
				// char gets promoted to int for va_arg!
				// clean it.
				char c = (char)(va_arg(ap, int) & ~0xFFFFFF00);
				a++;
				i++;
				continue;
			}
			default:
				break;
			}
		}
		else {
			a++;
		}
	}
	return a;
}
void _abc()
{

}

//Format and Print to the text mode screen using video.c
void __kprintf_va_list(char* str, va_list ap)
{
	//mutex_lock(&m_kprintf);
	char* s = 0;
	//cd->putc('c');
	for (size_t i = 0; i < 4096; i++)
	{
		if (str[i] == 0) break;
		if (str[i] == '%')
		{
			switch (str[i + 1])
			{
				/** string **/
			case 's':
				s = va_arg(ap, char*);
				printString(s);
				i++;
				continue;
				/** decimal **/
			case 'd': {
				int c = va_arg(ap, int);
				char str[32] = { 0 };
				__itoa_s(c, 10, str);
				printString(str);
				i++;
				continue;
			}
			case 'x': {
				int c = va_arg(ap, int);
				char str[32] = { 0 };
				__itoa(c, 16, str);
				printString(str);
				i++;
				continue;
			}
					  /** character **/
			case 'c': {
				// char gets promoted to int for va_arg!
				// clean it.
				char c = (char)(va_arg(ap, int) & ~0xFFFFFF00);
				printChar(c);
				i++;
				continue;
			}
			default:
				break;
			}
		}
		else {
			printChar(str[i]);
		}
	}
	va_end(ap);
}
int vsscanf(const char *str, const char *fmt, va_list args);
int sscanf(const char *str, const char *fmt, ...);

int vsnprintf(char *str, size_t size, const char *fmt, va_list args);
int snprintf(char *str, size_t size, const char *fmt, ...);

unsigned long int strtoul(const char *nptr, char **endptr, int base);
enum
{
	PAD_ZERO = 1,
	PAD_LEFT = 2,
};

typedef struct Formatter
{
	char *p;
	char *end;
	uint flags;
	int width;
} Formatter;

// ------------------------------------------------------------------------------------------------
static bool IsSpace(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v';
}

// ------------------------------------------------------------------------------------------------
static bool IsDigit(char c)
{
	return c >= '0' && c <= '9';
}

// ------------------------------------------------------------------------------------------------
static void OutputChar(Formatter *f, char c)
{
	if (f->p < f->end)
	{
		*f->p++ = c;
	}
}

// ------------------------------------------------------------------------------------------------
static void OutputString(Formatter *f, const char *s)
{
	int width = f->width;
	char padChar = f->flags & PAD_ZERO ? '0' : ' ';

	if (~f->flags & PAD_LEFT)
	{
		while (--width >= 0)
		{
			OutputChar(f, padChar);
		}
	}

	while (*s)
	{
		OutputChar(f, *s++);
	}

	while (--width >= 0)
	{
		OutputChar(f, padChar);
	}
}

// ------------------------------------------------------------------------------------------------
static void OutputDec(Formatter *f, unsigned long long n)
{
	char buf[32];
	char *end = buf + sizeof(buf) - 1;
	char *s = end;
	*s = '\0';
	uint hh = n;
	do
	{
		char c = '0' + (hh % 10);
		*--s = c;
		hh = hh / 10;
	} while (hh > 0);

	f->width -= end - s;
	OutputString(f, s);
}

// ------------------------------------------------------------------------------------------------
static void OutputHex(Formatter *f, char type, unsigned long long n)
{
	char buf[32];
	char *end = buf + sizeof(buf) - 1;
	char *s = end;
	*s = '\0';

	do
	{
		uint digit = n & 0xf;
		char c;
		if (digit < 10)
		{
			c = '0' + digit;
		}
		else if (type == 'x')
		{
			c = 'a' + digit - 10;
		}
		else
		{
			c = 'A' + digit - 10;
		}

		*--s = c;
		n >>= 4;
	} while (n > 0);

	f->width -= end - s;
	OutputString(f, s);
}

// ------------------------------------------------------------------------------------------------
static void OutputPointer(Formatter *f, void *p)
{
	unsigned long long n = (uintptr_t)p;
	OutputHex(f, 'x', n);
}

// ------------------------------------------------------------------------------------------------
int vsnprintf(char *str, size_t size, const char *fmt, va_list args)
{
	Formatter f;
	f.p = str;
	f.end = str + size - 1;

	for (;;)
	{
		// Read next character
		char c = *fmt++;
		if (!c)
		{
			break;
		}

		// Output non-format character
		if (c != '%')
		{
			OutputChar(&f, c);
			continue;
		}

		// Parse type specifier
		c = *fmt++;

		// Parse flags
		f.flags = 0;
		if (c == '-')
		{
			f.flags |= PAD_LEFT;
			c = *fmt++;
		}
		else if (c == '0')
		{
			f.flags |= PAD_ZERO;
			c = *fmt++;
		}

		// Parse width
		f.width = -1;
		int ww = 0;
		if (c == '.') {
			ww = 1;
			c = *fmt++;
		}
		if (IsDigit(c))
		{
			int width = 0;
			do
			{
				width = width * 10 + c - '0';
				c = *fmt++;
			} while (IsDigit(c));

			f.width = width;
		}

		// Parse length modifier
		bool isLongLong = false;

		if (c == 'l')
		{
			c = *fmt++;
			if (c == 'l')
			{
				c = *fmt++;
				isLongLong = true;
			}
		}


		// Process type specifier
		char type = c;
		switch (type)
		{
		case '%':
			OutputChar(&f, '%');
			break;

		case 'c':
			c = va_arg(args, int);
			OutputChar(&f, c);
			break;

		case 's':
		{
			char *s = va_arg(args, char *);
			if (!s)
			{
				s = "(null)";
			}

			if (f.width > 0)
			{
				char *p = s;
				while (*p)
				{
					++p;
				}


				if (ww)
				{
					int width = f.width;
					//printTextToWindow(1,mywin,"[%d]", f.width);
					char padChar = f.flags & PAD_ZERO ? '0' : ' ';

					if ((f.flags & PAD_LEFT))
					{
						while (--width >= 0)
						{
							OutputChar(&f, padChar);
						}
					}

					while (*s&&width > 0)
					{
						OutputChar(&f, *s++);
						width--;

					}

					while (--width >= 0)
					{
						OutputChar(&f, padChar);
					}

				}
				else {
					f.width -= p - s;
					OutputString(&f, s);
				}
			}
			else
				OutputString(&f, s);
		}
		break;

		case 'd':
		{
			long long n;
			if (isLongLong)
			{
				n = va_arg(args, long long);
			}
			else
			{
				n = va_arg(args, int);
			}

			if (n < 0)
			{
				OutputChar(&f, '-');
				n = -n;
			}

			OutputDec(&f, n);
		}
		break;

		case 'u':
		{
			unsigned long long n;
			if (isLongLong)
			{
				n = va_arg(args, unsigned long long);
			}
			else
			{
				n = va_arg(args, unsigned int);
			}

			OutputDec(&f, n);
		}
		break;

		case 'x':
		case 'X':
		{
			unsigned long long n;
			if (isLongLong)
			{
				n = va_arg(args, unsigned long long);
			}
			else
			{
				n = va_arg(args, unsigned int);
			}

			OutputHex(&f, type, n);
		}
		break;

		case 'p':
		{
			void *p = va_arg(args, void *);

			OutputChar(&f, '0');
			OutputChar(&f, 'x');
			OutputPointer(&f, p);
		}
		break;
		}
	}

	if (f.p < f.end + 1)
	{
		*f.p = '\0';
	}

	return f.p - str;
}

// ------------------------------------------------------------------------------------------------
int snprintf(char *str, size_t size, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	int len = vsnprintf(str, size, fmt, args);
	va_end(args);

	return len;
}

// ------------------------------------------------------------------------------------------------
int vsscanf(const char *str, const char *fmt, va_list args)
{
	int count = 0;

	for (;;)
	{
		// Read next character
		char c = *fmt++;
		if (!c)
		{
			break;
		}
		if (IsSpace(c))
		{
			// Whitespace
			while (IsSpace(*str))
			{
				++str;
			}
		}
		else if (c != '%')
		{
		match_literal:
			// Non-format character
			if (*str == '\0')
			{
				goto end_of_input;
			}

			if (*str != c)
			{
				goto match_failure;
			}

			++str;
		}
		else
		{
			// Parse type specifider
			c = *fmt++;

			// Process type specifier
			char type = c;
			switch (type)
			{
			case '%':
				goto match_literal;

			case 'd':
			{
				int sign = 1;

				c = *str++;
				if (c == '\0')
					goto end_of_input;

				if (c == '-')
				{
					sign = -1;
					c = *str++;
				}

				int n = 0;
				while (IsDigit(c))
				{
					n = n * 10 + c - '0';
					c = *str++;
				}

				n *= sign;
				--str;

				int *result = va_arg(args, int *);
				*result = n;
				++count;
			}
			break;
			}
		}
	}

match_failure:
	return count;

end_of_input:
	return count ? count : -1;
}

// ------------------------------------------------------------------------------------------------
int sscanf(const char *str, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	int count = vsscanf(str, fmt, args);
	va_end(args);

	return count;
}

// ------------------------------------------------------------------------------------------------
unsigned long int strtoul(const char *nptr, char **endptr, int base)
{
	const char *pCurrentChar = nptr;

	// Skip whitespace
	while (IsSpace(*pCurrentChar))
	{
		++pCurrentChar;
	}

	// Optionally there maybe a sign.
	bool neg = false;
	if (*pCurrentChar == '-')
	{
		neg = true;
		++pCurrentChar;
	}
	else if (*pCurrentChar == '+')
	{
		++pCurrentChar;
	}

	if (base == 0)
	{
		// detect base;
		if (*pCurrentChar == '0')
		{
			++pCurrentChar;
			if (*pCurrentChar == 'x')
			{
				base = 16;
				++pCurrentChar;
			}
			else
			{
				base = 8;
			}
		}
		else
		{
			base = 10;
		}
	}
	else if (base == 16)
	{
		if (*pCurrentChar == '0' && *(pCurrentChar + 1) == 'x')
		{
			pCurrentChar += 2;
		}
	}
	// Don't really need to skip leading 0 for oct.


	// I've not worried about limit error handling
	unsigned long int result = 0;
	bool done = false;
	while (!done)
	{
		char currentChar = *pCurrentChar;
		int  currentValue = 0;
		if (currentChar >= '0' && currentChar <= '9')
		{
			currentValue = currentChar - '0';
		}
		else if (currentChar >= 'a' && currentChar <= 'z')
		{
			currentValue = currentChar - 'a' + 10;
		}
		else if (currentChar >= 'A' && currentChar <= 'Z')
		{
			currentValue = currentChar - 'A' + 10;
		}
		else
		{
			done = true;
		}

		if (!done)
		{
			if (currentValue >= base)
			{
				done = true;
			}
			else
			{
				++pCurrentChar;
				result *= base;
				result += currentValue;
			}
		}
	}

	if (neg)
	{
		result = -result;
	}

	if (endptr != 0)
	{
		*endptr = (char *)pCurrentChar;
	}

	return result;
}
#pragma GCC pop_options
