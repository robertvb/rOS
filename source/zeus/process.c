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

    //sConsoleWrite(getCurrentSConsole(),"Starting process 1 ");

    int to = 10;
    int i;
    for (i=0; i<to; i++) {
    	//sConsoleWrite(getCurrentSConsole(),"PROCESO1........");
    }


    // Call software interrupt #0 (terminate)
    // asm volatile("SWI #0");
    terminate_process();
}

// Just some counting for easy debug on the screen. Simulate user process.
void sample_process_2() {

	uart_puts("Starting process 2 y haciendo swi1!\n\r");

    volatile char * string = "HELLO SW1 ! \n\r\n\r\0";
    unsigned int addr = (unsigned int) string;
	asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
    asm volatile("SWI #1");
    asm volatile("SWI #6");

	asm volatile("SWI #0");
/*
	uart_puts("Starting process 2!\n\r");

    int to = 200;
    int i;
    for (i=0; i<to; i++) {
    	uart_puts("PROCESO2. VALOR DE I = \n\r");
    	uart_puts(uintToString((unsigned int) i,DECIMAL));
    	uart_puts("\n\r");
    }


    // Call software interrupt #0 (terminate)
    // asm volatile("SWI #0");
    terminate_process();*/
}

void sample_process_console() {

	unsigned int getNextComand = 1;
	unsigned char command[500];
	command[0] = '\0';

    while (getNextComand) {

    }
}
