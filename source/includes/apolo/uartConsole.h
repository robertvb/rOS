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

#ifndef UARTCONSOLE_H
#define UARTCONSOLE_H

#include "commandInterpreter.h"
#include "../hades/rpi-uart.h"

#define MAX_UCONSOLE_INBUFFER 50
static const char prompt[] = "rOS232> ";

typedef struct {
	commandInterpreter_t commandInter;
	char inBuffer[MAX_UCONSOLE_INBUFFER];
}uartConsole_t;

void init_uartConsole(void);
void readCommand(void);
void activateUartConsole(void);

#endif /* SOURCE_INCLUDES_APOLO_UARTCONSOLE_H_ */
