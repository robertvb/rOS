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

#include "screenbackground.h"
#include "../zeus/procUtils.h"

#define MAX_SCONSOLE_INBUFFER 	50
#define MAX_SCONSOLES 		  	5 /* +1 BackGround */
static char * sprompt = " rOS- ";
#define SPROMPT_LEN 			6
#define LINE_SPACING			0
#define MAX_SCONSOLE_LINES		MAX_VERTICAL_PIXELS / CHAR_HEIGHT - 6  /* 768 / 16 - 6 = 42 CARACTERES */
#define MAX_SCONSOLE_LINE_LEN	MAX_HORIZONTAL_PIXELS / CHAR_WIDTH - SPROMPT_LEN - 2 /* 1024 / 8 - 2 = 126 CARACTERES */

typedef unsigned char SCid_t;

typedef struct {
	uint16_t backGroundColour;
	uint16_t fontColour;
	unsigned int currentX;
	unsigned int currentY;
	unsigned char matrixMessages[MAX_SCONSOLE_LINES][MAX_SCONSOLE_LINE_LEN];
	unsigned int messageCount;
	ProcessQueue_t bloquedProcs;
}ScreenConsole_t;

void init_screen_consoles(void);
SCid_t getCurrentSConsole(void);
void setSConsoleFontColour(SCid_t consoleNum, uint16_t colour);
void setSConsolebackGroundColour(SCid_t consoleNum, uint16_t colour);
void clearSConsole(SCid_t consoleNum);
void focusSConsole(SCid_t consoleNum);
void sConsoleWrite(SCid_t consoleNum, char * str);
void sConsoleManageChar(char c);
void sConsoleManageBlinkPrompt(void);

#endif /* SOURCE_INCLUDES_APOLO_SCREENCONSOLE_H_ */
