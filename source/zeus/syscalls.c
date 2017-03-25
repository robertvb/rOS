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


#include "../includes/zeus/scheduler.h"
#include "../includes/zeus/syscalls.h"
/*
 * Rutina de manejo de interrupciones software
 */
void syscall(unsigned int swi) {

	uart_puts("Handling syscall: ");
	uart_puts("\n\r");

	switch (swi) {

		case SYSCALL_TERMINATE_PROCESS:
			uart_puts("Invoking syscall terminate_process()");
			uart_puts("\n\r");

			// LLamada a la rutina intra-kernel pertinente
			terminate_process();
			break;
	}

	uart_puts("Turning interrupt on again");
	uart_puts("\n\r");

	// Se rehabilitan las interrupciones
	asm volatile("cpsie i");

	// Esperar a la siguiente interrupción.
	// (provisional, Esto problamente no sea lo mas indicado TODO)....
	halt();

}