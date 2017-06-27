/*
Copyright (c) 2015 Robert A. Vazeux Blanco

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
 */

#include "../includes/apolo/debugTracer.h"

static unsigned int level;
static char * promptL1 = "[DEBUG:1]> ";
static char * promptL2 = "[DEBUG:2]> ";
static char * promptL3 = "[DEBUG:3]> ";

void init_debugTracer(void) {
	level = 1;
}

void debugPrintStrV1(char * str) {
	if(level == 1) {
		uart_puts(promptL1);
		uart_puts(str);
	}
}

void debugPrintStrV2(char * str) {
	if(level <= 2) {
		uart_puts(promptL2);
		uart_puts(str);
	}
}

void debugPrintStrV3(char * str) {
	if(level <= 3) {
		uart_puts(promptL3);
		uart_puts(str);
	}
}

void debugPrintValueV1(unsigned int value, char base) {
	if(level == 1) {
		uart_puts(uintToString(value,base));
	}
}

void debugPrintValueV2(unsigned int value, char base) {
	if(level <= 2) {
		uart_puts(uintToString(value,base));
	}
}

void debugPrintValueV3(unsigned int value, char base) {
	if(level <=3) {
		uart_puts(uintToString(value,base));
	}
}
