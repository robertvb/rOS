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

#include "../includes/apolo/screenConsole.h"

static screenConsole_t sConsoleList[MAX_SCONSOLES];
static unsigned char currentSConsole;

void init_screen_consoles(void) {

	char i;
	for(i = 0; i < MAX_SCONSOLES; i++) {
		sConsoleList[i].backGroundColour = Green;
		sConsoleList[i].fontColour = White;
		sConsoleList[i].currentX = 3*CHAR_WIDTH;
		sConsoleList[i].currentX = 2*CHAR_HEIGHT;
		clearSConsole(i);
	}

	currentSConsole = 0;
}

unsigned char getNextSConsole(void) {
	return currentSConsole = (currentSConsole) + 1 % MAX_SCONSOLES;
}

void clearSConsole(unsigned char consoleNum) {

	if(consoleNum >= MAX_SCONSOLES) {
		return;
	}

	rpi_gpu_framebuffer_descriptor_t* FrameBufferDescrp  = (rpi_gpu_framebuffer_descriptor_t *) RPI_GetFrameBufferDescpr();
	uint16_t * pixel = (uint16_t *) sConsoleList[consoleNum].frameBuffer;

	uint32_t x,y;
	for(y = 0; y < FrameBufferDescrp->vHeight; y++) {
		for(x = 0; x < FrameBufferDescrp->vWidth; x++) {
			// ponemos a backGroundColour todos menos los bordes TODO
			*(pixel++) = sConsoleList[consoleNum].backGroundColour;
		}
	}

}

void focusSConsole(unsigned char consoleNum) {

	rpi_gpu_framebuffer_descriptor_t * frameBufferDscr  = (rpi_gpu_framebuffer_descriptor_t*) RPI_GetFrameBufferDescpr();
	uint16_t * pixel = (uint16_t *) frameBufferDscr->pointer;
	uint16_t * cPixel = (uint16_t *) sConsoleList[consoleNum].frameBuffer;
	uint32_t x,y;
	for(y = 0; y < frameBufferDscr->vHeight; y++)
		for(x = 0; x < frameBufferDscr->vWidth; x++)
			*(pixel++) = *(cPixel++);
}

void sConsoleWrite(unsigned char consoleNum, char * str) {

	const unsigned int len = stringLength(str);

	drawStringFB(sConsoleList[consoleNum].frameBuffer,prompt,5,sConsoleList[consoleNum].currentX,sConsoleList[consoleNum].currentY);
	sConsoleList[consoleNum].currentX+=5*CHAR_WIDTH;
	drawStringFB(sConsoleList[consoleNum].frameBuffer,str,len,sConsoleList[consoleNum].currentX,sConsoleList[consoleNum].currentY);
	sConsoleList[consoleNum].currentY+=2*CHAR_HEIGHT;

}
