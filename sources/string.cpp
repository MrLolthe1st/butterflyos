#include "includes/string.h"
#include "includes/memory/memory.h"
#define NULL 0
#define int32_t int
#define u_int unsigned int
#define u_long unsigned int
#define u_int32_t unsigned int
#define u_quad_t unsigned int
#define quad_t int
static bool isspace(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v';
}


char * strccat(char *dst, int c) {
	int len;

	if (!dst)
		return (dst);
	len = strlen(dst);
	dst[len] = c;
	dst[len + 1] = 0;
	return (dst);
}

char * strcat(char *dst, const char *src) {
	char *d;

	if (!dst || !src)
		return (dst);

	d = dst;
	for (; *d; d++);
	for (; *src; src++)
		*d++ = *src;
	*d = 0;
	return (dst);
}
/*
 * Compare strings.
 */
int strcmp(const char *s1, const char *s2) {
	while (*s1 == *s2++)
		if (*s1++ == 0)
			return (0);
	return (*(const unsigned char *)s1 - *(const unsigned char *)--s2);
}

char * strcpy(char *d, const char *s) {
	strlcpy(d, s, 2147483647);
	return d;
}

size_t strlcpy(char *d, const char *s, size_t l) {
	size_t len = l;

	if (d == NULL || l == 0)
		return 0;
	*d = 0;
	if (s == NULL)
		return 0;

	while (--len)
		if ((*d++ = *s++) == 0)
			break;
	if (len == 0)
		*d = 0;
	len = l - len;
	return len;
}

size_t strlen(const char *p) {
	int n;

	if (!p)
		return (0);
	for (n = 0; *p; p++)
		n++;
	return (n);
}

void strmerge(char *d, const char *s) {
	int i;

	if (strlen(d) < strlen(s)) {
		for (i = strlen(d); i < strlen(s); i++)
			d[i] = ' ';
		d[i] = 0;
	}
	for (; *d && *s; d++, s++)
		if (*s != ' ')
			*d = *s;
}

char * strncat(char *dst, const char *src, size_t n) {
	char *d;

	if (!dst || !src)
		return (dst);
	d = dst;
	for (; *d; d++);
	for (; *src && n; src++, n--)
		*d++ = *src;
	*d = 0;
	return (dst);
}


char * strnchr(const char *p, char c, size_t n) {
	if (!p)
		return (0);

	while (n-- > 0) {
		if (*p == c)
			return ((char *)p);
		p++;
	}
	return (0);
}

int strncmp(const char *s1, const char *s2, size_t n) {
	if (!s1 || !s2)
		return (0);

	while (n && (*s1 == *s2)) {
		if (*s1 == 0)
			return (0);
		s1++;
		s2++;
		n--;
	}
	if (n)
		return (*s1 - *s2);
	return (0);
}


char * strncpy(char *dst, const char *src, size_t n) {
	char *d;

	if (!dst || !src)
		return (dst);
	d = dst;
	for (; *src && n; d++, src++, n--)
		*d = *src;
	while (n--)
		*d++ = '\0';
	return (dst);
}

int strnwrd(const char *p) {
	int n;

	if (!p)
		return (0);

	for (n = 0; *p; n++) {
		while (isspace(*p))
			p++;
		if (!*p)
			return (n);
		while (!isspace(*p) && *p != 0)
			p++;
	}
	return (n);
}

char * strpbrk(const char *s1, const char *s2) {
	for (; *s1; s1++) {
		if (strchr(s2, *s1) != 0)
			return ((char *)s1);
	}
	return (0);
}

char * strposn(const char *p, const char *q) {
	const char *s, *t;

	if (!p || !q)
		return (0);

	if (!*q)
		return ((char *)(p + strlen(p)));
	for (; *p; p++) {
		if (*p == *q) {
			t = p;
			s = q;
			for (; *t; s++, t++) {
				if (*t != *s)
					break;
			}
			if (!*s)
				return ((char *)p);
		}
	}
	return (0);
}

char * strchr(const char *p, int c) {
	if (!p)
		return (0);

	for (; *p; p++)
		if (*p == c)
			return ((char *)p);
	return (0);
}

int isdigit(char c) {
	return c >= '0'&&c <= '9';
}

char * strichr(char *p, int c) {
	char *t;

	if (p != NULL) {
		for (t = p; *t; t++);
		for (; t >= p; t--) {
			*(t + 1) = *t;
		}
		*p = c;
	}
	return (p);
}




static char *
_getbase(char *p, int *basep)
{
	if (p[0] == '0') {
		switch (p[1]) {
		case 'x':
			*basep = 16;
			break;
		case 't': case 'n':
			*basep = 10;
			break;
		case 'o':
			*basep = 8;
			break;
		default:
			*basep = 10;
			return (p);
		}
		return (p + 2);
	}
	*basep = 10;
	return (p);
}


/*
 *  _atob(vp,p,base)
 */
static int
_atob(u_quad_t *vp, char *p, int base)
{
	u_quad_t value, v1, v2;
	char *q, tmp[20];
	int digit;

	if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
		base = 16;
		p += 2;
	}

	if (base == 16 && (q = strchr(p, '.')) != 0) {
		if (q - p > sizeof(tmp) - 1)
			return (0);

		strncpy(tmp, p, q - p);
		tmp[q - p] = '\0';
		if (!_atob(&v1, tmp, 16))
			return (0);

		q++;
		if (strchr(q, '.'))
			return (0);

		if (!_atob(&v2, q, 16))
			return (0);
		*vp = (v1 << 16) + v2;
		return (1);
	}

	value = *vp = 0;
	for (; *p; p++) {
		if (*p >= '0' && *p <= '9')
			digit = *p - '0';
		else if (*p >= 'a' && *p <= 'f')
			digit = *p - 'a' + 10;
		else if (*p >= 'A' && *p <= 'F')
			digit = *p - 'A' + 10;
		else
			return (0);

		if (digit >= base)
			return (0);
		value *= base;
		value += digit;
	}
	*vp = value;
	return (1);
}

/*
 *  atob(vp,p,base)
 *      converts p to binary result in vp, rtn 1 on success
 */
int
atob(u_int32_t *vp, char *p, int base)
{
	u_quad_t v;

	if (base == 0)
		p = _getbase(p, &base);
	if (_atob(&v, p, base)) {
		*vp = v;
		return (1);
	}
	return (0);
}


/*
 *  llatob(vp,p,base)
 *      converts p to binary result in vp, rtn 1 on success
 */
int
llatob(u_quad_t *vp, char *p, int base)
{
	if (base == 0)
		p = _getbase(p, &base);
	return _atob(vp, p, base);
}


/*
 *  char *btoa(dst,value,base)
 *      converts value to ascii, result in dst
 */
char *
btoa(char *dst, u_int value, int base)
{
	char buf[34], digit;
	int i, j, rem, neg;

	if (value == 0) {
		dst[0] = '0';
		dst[1] = 0;
		return (dst);
	}

	neg = 0;
	if (base == -10) {
		base = 10;
		if (value & (1L << 31)) {
			value = (~value) + 1;
			neg = 1;
		}
	}

	for (i = 0; value != 0; i++) {
		rem = value % base;
		value /= base;
		if (rem >= 0 && rem <= 9)
			digit = rem + '0';
		else if (rem >= 10 && rem <= 36)
			digit = (rem - 10) + 'a';
		buf[i] = digit;
	}

	buf[i] = 0;
	if (neg)
		strcat(buf, "-");

	/* reverse the string */
	for (i = 0, j = strlen(buf) - 1; j >= 0; i++, j--)
		dst[i] = buf[j];
	dst[i] = 0;
	return (dst);
}

/*
 *  char *btoa(dst,value,base)
 *      converts value to ascii, result in dst
 */
char * llbtoa(char *dst, u_quad_t value, int base) {
	char buf[66], digit;
	int i, j, rem, neg;

	if (value == 0) {
		dst[0] = '0';
		dst[1] = 0;
		return (dst);
	}

	neg = 0;
	if (base == -10) {
		base = 10;
		if (value & (1LL << 63)) {
			value = (~value) + 1;
			neg = 1;
		}
	}

	for (i = 0; value != 0; i++) {
		rem = value % base;
		value /= base;
		if (rem >= 0 && rem <= 9)
			digit = rem + '0';
		else if (rem >= 10 && rem <= 36)
			digit = (rem - 10) + 'a';
		buf[i] = digit;
	}

	buf[i] = 0;
	if (neg)
		strcat(buf, "-");

	/* reverse the string */
	for (i = 0, j = strlen(buf) - 1; j >= 0; i++, j--)
		dst[i] = buf[j];
	dst[i] = 0;
	return (dst);
}

/*
 *  gethex(vp,p,n)
 *      convert n hex digits from p to binary, result in vp,
 *      rtn 1 on success
 */
int gethex(int32_t *vp, char *p, int n) {
	u_long v;
	int digit;

	for (v = 0; n > 0; n--) {
		if (*p == 0)
			return (0);
		if (*p >= '0' && *p <= '9')
			digit = *p - '0';
		else if (*p >= 'a' && *p <= 'f')
			digit = *p - 'a' + 10;
		else if (*p >= 'A' && *p <= 'F')
			digit = *p - 'A' + 10;
		else
			return (0);

		v <<= 4;
		v |= digit;
		p++;
	}
	*vp = v;
	return (1);
}





#define FMT_RJUST 0x02
#define FMT_LJUST 0x03
#define FMT_RJUST0 0x04
#define FMT_CENTER 0x05
void str_fmt(char *p, int size, int fmt)
{
	int n, m, len;

	len = strlen(p);
	switch (fmt) {
	case FMT_RJUST:
		for (n = size - len; n > 0; n--)
			strichr(p, ' ');
		break;
	case FMT_LJUST:
		for (m = size - len; m > 0; m--)
			strcat(p, " ");
		break;
	case FMT_RJUST0:
		for (n = size - len; n > 0; n--)
			strichr(p, '0');
		break;
	case FMT_CENTER:
		m = (size - len) / 2;
		n = size - (len + m);
		for (; m > 0; m--)
			strcat(p, " ");
		for (; n > 0; n--)
			strichr(p, ' ');
		break;
	}
}

int
toupper(int c)
{
	if (c < 'a' || c > 'z')
		return c;
	else
		return c - 0x20;
}
char* strtoupper(char* s) {
	char* p = s;
	while (*p != '\0') {
		*p = toupper(*p);
		p++;
	}

	return s;
}


/*
 *  int vsprintf(d,s,ap)
 */
int vsprintf(char *d, const char *s, va_list ap)
{
	const char *t;
	char *p, *dst, tmp[40];
	unsigned int n;
	int fmt, trunc, haddot, width, base, longlong;
#ifdef FLOATINGPT
	double dbl;

#ifndef NEWFP
	EP ex;
#endif
#endif

	dst = d;
	for (; *s;) {
		if (*s == '%') {
			s++;
			fmt = FMT_RJUST;
			width = trunc = haddot = longlong = 0;
			for (; *s; s++) {
				if (strchr("bcdefgilopPrRsuxX%", *s))
					break;
				else if (*s == '-')
					fmt = FMT_LJUST;
				else if (*s == '0')
					fmt = FMT_RJUST0;
				else if (*s == '~')
					fmt = FMT_CENTER;
				else if (*s == '*') {
					if (haddot)
						trunc = va_arg(ap, int);
					else
						width = va_arg(ap, int);
				}
				else if (*s >= '1' && *s <= '9') {
					for (t = s; isdigit(*s); s++);
					strncpy(tmp, t, s - t);
					tmp[s - t] = '\0';
					atob(&n, tmp, 10);
					if (haddot)
						trunc = n;
					else
						width = n;
					s--;
				}
				else if (*s == '.')
					haddot = 1;
			}
			if (*s == '%') {
				*d++ = '%';
				*d = 0;
			}
			else if (*s == 's') {
				p = va_arg(ap, char *);

				if (p)
					strcpy(d, p);
				else
					strcpy(d, "(null)");
			}
			else if (*s == 'c') {
				n = va_arg(ap, int);
				*d = n;
				d[1] = 0;
			}
			else {
				if (*s == 'l') {
					if (*++s == 'l') {
						longlong = 1;
						++s;
					}
				}
				if (strchr("bdiopPrRxXu", *s)) {
					if (*s == 'd' || *s == 'i')
						base = -10;
					else if (*s == 'u')
						base = 10;
					else if (*s == 'x' || *s == 'X')
						base = 16;
					else if (*s == 'p' || *s == 'P') {
						base = 16;
						if (*s == 'p') {
							*d++ = '0';
							*d++ = 'x';
						}
						fmt = FMT_RJUST0;
						if (sizeof(long) > 4) {
							width = 16;
							longlong = 1;
						}
						else {
							width = 8;
						}
					}
					else if (*s == 'r' || *s == 'R') {
						base = 16;
						if (*s == 'r') {
							*d++ = '0';
							*d++ = 'x';
						}
						fmt = FMT_RJUST0;
						if (sizeof(size_t) > 4) {
							width = 16;
							longlong = 1;
						}
						else {
							width = 8;
						}
					}
					else if (*s == 'o')
						base = 8;
					else if (*s == 'b')
						base = 2;
					if (longlong)
						llbtoa(d, va_arg(ap, quad_t),
							base);
					else
						btoa(d, va_arg(ap, int), base);

					if (*s == 'X')
						strtoupper(d);
				}
#ifdef FLOATINGPT
				else if (strchr("eEfgG", *s)) {
					static void dtoa(char *, double, int, int, int);
					dbl = va_arg(ap, double);
					dtoa(d, dbl, *s, width, trunc);
					trunc = 0;
				}
#endif
			}
			if (trunc)
				d[trunc] = 0;
			if (width)
				str_fmt(d, width, fmt);
			for (; *d; d++);
			s++;
		}
		else
			*d++ = *s++;
	}
	*d = 0;
	return (d - dst);
}

#ifdef FLOATINGPT
/*
 * Floating point output, cvt() onward lifted from BSD sources:
 *
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#define MAX_FCONVERSION	512	/* largest possible real conversion 	*/
#define MAX_EXPT	5	/* largest possible exponent field */
#define MAX_FRACT	39	/* largest possible fraction field */

#define TESTFLAG(x)	0


typedef double rtype;

extern double modf(double, double *);
#define	to_char(n)	((n) + '0')
#define	to_digit(c)	((c) - '0')
#define _isNan(arg)	((arg) != (arg))

static int cvt(rtype arg, int prec, char *signp, int fmtch,
	char *startp, char *endp);
static char *round(double fract, int *exp, char *start, char *end,
	char ch, char *signp);
static char *exponent(char *p, int exp, int fmtch);


/*
 * _finite arg not Infinity or Nan
 */
static int _finite(rtype d)
{
#if ENDIAN == ENDIAN_LITTLE
	struct IEEEdp {
		unsigned manl : 32;
		unsigned manh : 20;
		unsigned exp : 11;
		unsigned sign : 1;
	} *ip;
#else
	struct IEEEdp {
		unsigned sign : 1;
		unsigned exp : 11;
		unsigned manh : 20;
		unsigned manl : 32;
	} *ip;
#endif

	ip = (struct IEEEdp *)&d;
	return (ip->exp != 0x7ff);
}


static void dtoa(char *dbuf, rtype arg, int fmtch, int width, int prec)
{
	char	buf[MAX_FCONVERSION + 1], *cp;
	char	sign;
	int	size;

	if (!_finite(arg)) {
		if (_isNan(arg))
			strcpy(dbuf, "NaN");
		else if (arg < 0)
			strcpy(dbuf, "-Infinity");
		else
			strcpy(dbuf, "Infinity");
		return;
	}

	if (prec == 0)
		prec = 6;
	else if (prec > MAX_FRACT)
		prec = MAX_FRACT;

	/* leave room for sign at start of buffer */
	cp = buf + 1;

	/*
	 * cvt may have to round up before the "start" of
	 * its buffer, i.e. ``intf("%.2f", (double)9.999);'';
	 * if the first character is still NUL, it did.
	 * softsign avoids negative 0 if _double < 0 but
	 * no significant digits will be shown.
	 */
	*cp = '\0';
	size = cvt(arg, prec, &sign, fmtch, cp, buf + sizeof(buf));
	if (*cp == '\0')
		cp++;

	if (sign)
		*--cp = sign, size++;

	cp[size] = 0;
	memcpy(dbuf, cp, size + 1);
}


static int
cvt(rtype number, int prec, char *signp, int fmtch, char *startp, char *endp)
{
	register char *p, *t;
	register double fract;
	double integer, tmp;
	int dotrim, expcnt, gformat;

	dotrim = expcnt = gformat = 0;
	if (number < 0) {
		number = -number;
		*signp = '-';
	}
	else
		*signp = 0;

	fract = modf(number, &integer);

	/* get an extra slot for rounding. */
	t = ++startp;

	/*
	 * get integer portion of number; put into the end of the buffer; the
	 * .01 is added for modf(356.0 / 10, &integer) returning .59999999...
	 */
	for (p = endp - 1; integer; ++expcnt) {
		tmp = modf(integer / 10, &integer);
		*p-- = to_char((int)((tmp + .01) * 10));
	}
	switch (fmtch) {
	case 'f':
		/* reverse integer into beginning of buffer */
		if (expcnt)
			for (; ++p < endp; *t++ = *p);
		else
			*t++ = '0';
		/*
		 * if precision required or alternate flag set, add in a
		 * decimal point.
		 */
		if (prec || TESTFLAG(ALTERNATE_FORM))
			*t++ = '.';
		/* if requires more precision and some fraction left */
		if (fract) {
			if (prec)
				do {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((int)tmp);
				} while (--prec && fract);
				if (fract)
					startp = round(fract, (int *)NULL, startp,
						t - 1, (char)0, signp);
		}
		for (; prec--; *t++ = '0');
		break;
	case 'e':
	case 'E':
	eformat:	if (expcnt) {
		*t++ = *++p;
		if (prec || TESTFLAG(ALTERNATE_FORM))
			*t++ = '.';
		/* if requires more precision and some integer left */
		for (; prec && ++p < endp; --prec)
			*t++ = *p;
		/*
		 * if done precision and more of the integer component,
		 * round using it; adjust fract so we don't re-round
		 * later.
		 */
		if (!prec && ++p < endp) {
			fract = 0;
			startp = round((double)0, &expcnt, startp,
				t - 1, *p, signp);
		}
		/* adjust expcnt for digit in front of decimal */
		--expcnt;
	}
				/* until first fractional digit, decrement exponent */
				else if (fract) {
		/* adjust expcnt for digit in front of decimal */
		for (expcnt = -1;; --expcnt) {
			fract = modf(fract * 10, &tmp);
			if (tmp)
				break;
		}
		*t++ = to_char((int)tmp);
		if (prec || TESTFLAG(ALTERNATE_FORM))
			*t++ = '.';
	}
				else {
		*t++ = '0';
		if (prec || TESTFLAG(ALTERNATE_FORM))
			*t++ = '.';
	}
				/* if requires more precision and some fraction left */
				if (fract) {
					if (prec)
						do {
							fract = modf(fract * 10, &tmp);
							*t++ = to_char((int)tmp);
						} while (--prec && fract);
						if (fract)
							startp = round(fract, &expcnt, startp,
								t - 1, (char)0, signp);
				}
				/* if requires more precision */
				for (; prec--; *t++ = '0');

				/* unless alternate flag, trim any g/G format trailing 0's */
				if (gformat && !TESTFLAG(ALTERNATE_FORM)) {
					while (t > startp && *--t == '0');
					if (*t == '.')
						--t;
					++t;
				}
				t = exponent(t, expcnt, fmtch);
				break;
	case 'g':
	case 'G':
		/* a precision of 0 is treated as a precision of 1. */
		if (!prec)
			++prec;
		/*
		 * ``The style used depends on the value converted; style e
		 * will be used only if the exponent resulting from the
		 * conversion is less than -4 or greater than the precision.''
		 *	-- ANSI X3J11
		 */
		if (expcnt > prec || (!expcnt && fract && fract < .0001)) {
			/*
			 * g/G format counts "significant digits, not digits of
			 * precision; for the e/E format, this just causes an
			 * off-by-one problem, i.e. g/G considers the digit
			 * before the decimal point significant and e/E doesn't
			 * count it as precision.
			 */
			--prec;
			fmtch -= 2;		/* G->E, g->e */
			gformat = 1;
			goto eformat;
		}
		/*
		 * reverse integer into beginning of buffer,
		 * note, decrement precision
		 */
		if (expcnt)
			for (; ++p < endp; *t++ = *p, --prec);
		else
			*t++ = '0';
		/*
		 * if precision required or alternate flag set, add in a
		 * decimal point.  If no digits yet, add in leading 0.
		 */
		if (prec || TESTFLAG(ALTERNATE_FORM)) {
			dotrim = 1;
			*t++ = '.';
		}
		else
			dotrim = 0;
		/* if requires more precision and some fraction left */
		if (fract) {
			if (prec) {
				do {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((int)tmp);
				} while (!tmp && !expcnt);
				while (--prec && fract) {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((int)tmp);
				}
			}
			if (fract)
				startp = round(fract, (int *)NULL, startp,
					t - 1, (char)0, signp);
		}
		/* alternate format, adds 0's for precision, else trim 0's */
		if (TESTFLAG(ALTERNATE_FORM))
			for (; prec--; *t++ = '0');
		else if (dotrim) {
			while (t > startp && *--t == '0');
			if (*t != '.')
				++t;
		}
	}
	return (t - startp);
}


static char *
round(double fract, int *exp, char *start, char *end, char ch, char *signp)
{
	double tmp;

	if (fract)
		(void)modf(fract * 10, &tmp);
	else
		tmp = to_digit(ch);
	if (tmp > 4)
		for (;; --end) {
			if (*end == '.')
				--end;
			if (++*end <= '9')
				break;
			*end = '0';
			if (end == start) {
				if (exp) {	/* e/E; increment exponent */
					*end = '1';
					++*exp;
				}
				else {		/* f; add extra digit */
					*--end = '1';
					--start;
				}
				break;
			}
		}
	/* ``"%.3f", (double)-0.0004'' gives you a negative 0. */
	else if (*signp == '-')
		for (;; --end) {
			if (*end == '.')
				--end;
			if (*end != '0')
				break;
			if (end == start)
				*signp = 0;
		}
	return (start);
}

static char *
exponent(char *p, int exp, int fmtch)
{
	char *t;
	char expbuf[MAX_FCONVERSION];

	*p++ = fmtch;
	if (exp < 0) {
		exp = -exp;
		*p++ = '-';
	}
	else
		*p++ = '+';
	t = expbuf + MAX_FCONVERSION;
	if (exp > 9) {
		do {
			*--t = to_char(exp % 10);
		} while ((exp /= 10) > 9);
		*--t = to_char(exp);
		for (; t < expbuf + MAX_FCONVERSION; *p++ = *t++);
	}
	else {
		*p++ = '0';
		*p++ = to_char(exp);
	}
	return (p);
}
#endif /* FLOATINGPT */
void *memcpy(void *dest, const void *source, size_t count)
{
	char * c = (char*)dest;
	const char * s = (const char*)source;
	for (size_t i = 0; i < count; i++)
		*c++ = *s++;
}
void *memset(void *buf, int ch, size_t count)
{
	char * c = (char*)buf;
	for (size_t i = 0; i < count; i++)
		*c++ = ch;
}
char buffer[1024];
int printf(const char * str, ...)
{
	if (!str)
		return 0;
	va_list ap;
	va_start(ap, str);

	int res = vsprintf(buffer, (char *)str, ap);
	print_string(buffer);
	return res;
}
