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

#ifndef SOURCE_INCLUDES_APOLO_SCREENCONSOLE_H_
#define SOURCE_INCLUDES_APOLO_SCREENCONSOLE_H_

#include "commandInterpreter.h"
#include "screenbackground.h"

#define MAX_SCONSOLE_INBUFFER 50
#define MAX_SCONSOLES 		  5
static char * prompt = "rOS> ";

typedef struct {
	commandInterpreter_t * commandInter;
	uint16_t backGroundColour;
	uint16_t fontColour;
	unsigned int currentX;
	unsigned int currentY;
	unsigned char inBuffer[MAX_SCONSOLE_INBUFFER];
	unsigned char outBuffer[MAX_COMMAND_OUTPUT];
	unsigned int frameBuffer[0x180000];
}screenConsole_t;

void init_screen_consoles(void);
void clearSConsole(unsigned char consoleNum);
void focusSConsole(unsigned char consoleNum);
void sConsoleWrite(unsigned char consoleNum, char * str);

#endif /* SOURCE_INCLUDES_APOLO_SCREENCONSOLE_H_ */
