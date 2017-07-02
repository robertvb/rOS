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

#include "../includes/apolo/uartConsole.h"

static uartConsole_t uartConsole;

void init_uartConsole(void) {
	/* inicializamos buffer */
	int i = 0;
	while(i < MAX_UCONSOLE_INBUFFER) {
		uartConsole.inBuffer[i] = '\0';
		i++;
	}
	/* inicializamos el interprete de comandos */
	//init_commandInterpreter(&uartConsole.commandInter,uartConsole.outBuffer);
}
void readCommand(void) {
	uart_puts(prompt);
	char c = 1;
	int i,j = 0;
	for(i = 0; i < 500 && (c = uart_getc()) != '-'; i++) {
		uartConsole.inBuffer[i] = c;
	}
	/* se llama a executeCommand */
	executeCommand(uartConsole.inBuffer);
	/* la salida se encuentra en el buffer del interprete */
	uart_puts("\r\n");
}

void activateUartConsole(void) {
	while(1)
		readCommand();
}
