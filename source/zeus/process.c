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

#include "../includes/zeus/process.h"

// Just some counting for easy debug on the screen. Simulate user process.
void sample_process_1() {

    uart_puts("Starting process 1 ");

    int to = 300;

    int i, j;
    for (i=0; i<to; i++) {
        for (j=0; j<to*to; j++) {
			asm volatile("NOP");
		}
    }

    // Call software interrupt #0 (terminate)
    asm volatile("SWI #0");
}

// Just some counting for easy debug on the screen. Simulate user process.
void sample_process_2() {

	uart_puts("Starting process 2 ");

    int to = 300;

    int i, j;
    for (i=0; i<to; i++) {
        for (j=0; j<to*to; j++) {
			asm volatile("NOP");
		}
    }

    // Call software interrupt #0 (terminate)
    asm volatile("SWI #0");
}
