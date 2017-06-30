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

	currentSConsole = MAX_SCONSOLES; 	/* bgInfo */

	char i;
	for(i = 0; i < MAX_SCONSOLES; i++) {
		sConsoleList[i].backGroundColour = Black;
		sConsoleList[i].fontColour = White;
		sConsoleList[i].currentX = 0;
		sConsoleList[i].currentY = 3*CHAR_HEIGHT;
		sConsoleList[i].messageCount = 0;
		sConsoleList[i].commandInter = getCommandInterpreter();
		clearSConsole(i);
	}

}

unsigned char getCurrentSConsole(void) {
	return currentSConsole;
}

static unsigned char nextSConsole(void) {
	return currentSConsole = ((currentSConsole + 1) % (MAX_SCONSOLES + 1));
}

void clearSConsole(unsigned char consoleNum) {

	if(consoleNum >= MAX_SCONSOLES) {
		return;
	}

	uint32_t message;
	for(message = 0; message < MAX_SCONSOLE_LINE_LEN; message++) {
		sConsoleList[consoleNum].matrixMessages[message][0] = '\0';
	}

	if(consoleNum == currentSConsole) {

		rpi_gpu_framebuffer_descriptor_t * frameBufferDscr  = (rpi_gpu_framebuffer_descriptor_t*) RPI_GetFrameBufferDescpr();
		uint16_t * pixel = (uint16_t *) frameBufferDscr->pointer;
		uint32_t x,y;
		for(y = 0; y < frameBufferDscr->vHeight; y++)
			for(x = 0; x < frameBufferDscr->vWidth; x++) {
				if(y <= CHAR_HEIGHT) {
					*(pixel++) = LightGrey;
				} else {
					*(pixel++) = sConsoleList[consoleNum].backGroundColour;
				}
			}

		sConsoleList[consoleNum].currentX += drawStringCL(Black,"rOS v0.8 | Terminal ",20,0,0);
		drawStringCL(Black,uintToString(currentSConsole,DECIMAL),2,20*CHAR_WIDTH,0);
	}

}

void focusSConsole(unsigned char consoleNum) {

	if(consoleNum >= MAX_SCONSOLES) {
		return;
	}

	rpi_gpu_framebuffer_descriptor_t * frameBufferDscr  = (rpi_gpu_framebuffer_descriptor_t*) RPI_GetFrameBufferDescpr();
	uint16_t * pixel = (uint16_t *) frameBufferDscr->pointer;
	uint32_t x,y;
	for(y = 0; y < frameBufferDscr->vHeight; y++)
		for(x = 0; x < frameBufferDscr->vWidth; x++) {
			if(y <= CHAR_HEIGHT) {
				*(pixel++) = LightGrey;
			} else {
				*(pixel++) = sConsoleList[consoleNum].backGroundColour;
			}
		}

	sConsoleList[consoleNum].currentX += drawStringCL(Black,"rOS v0.8 | Terminal ",20,0,0);
	drawStringCL(Black,uintToString(currentSConsole,DECIMAL),2,20*CHAR_WIDTH,0);

	sConsoleList[consoleNum].currentX = 0;
	sConsoleList[consoleNum].currentY = 3*CHAR_HEIGHT;

	for(x = 0; x < sConsoleList[consoleNum].messageCount; x++) {
		unsigned char* str = sConsoleList[consoleNum].matrixMessages[x];
		unsigned int len = stringLength(str);
		if(len > 0) {
			drawStringCL(sConsoleList[consoleNum].fontColour,sprompt,SPROMPT_LEN,sConsoleList[consoleNum].currentX,sConsoleList[consoleNum].currentY);
			sConsoleList[consoleNum].currentX += SPROMPT_LEN*CHAR_WIDTH;

			drawStringCL(sConsoleList[consoleNum].fontColour,str,len,sConsoleList[consoleNum].currentX,sConsoleList[consoleNum].currentY);
			sConsoleList[consoleNum].currentY += CHAR_HEIGHT;

			sConsoleList[consoleNum].currentX = 0;
		}
	}

	drawStringCL(sConsoleList[consoleNum].fontColour,sprompt,SPROMPT_LEN,sConsoleList[consoleNum].currentX,sConsoleList[consoleNum].currentY);
	sConsoleList[consoleNum].currentX += SPROMPT_LEN*CHAR_WIDTH;

}

void sConsoleWrite(unsigned char consoleNum, char * str) {

	if(consoleNum >= MAX_SCONSOLES) {
		return;
	}

	const unsigned int len = stringLength(str);

	if(len > MAX_SCONSOLE_LINE_LEN) {
		return;
	}

	strncpy(sConsoleList[consoleNum].matrixMessages[sConsoleList[consoleNum].messageCount],(uint8_t *)str,len);

	sConsoleList[consoleNum].messageCount++;

	if(currentSConsole == consoleNum) {
		sConsoleList[consoleNum].currentX+= SPROMPT_LEN * CHAR_WIDTH;
		drawStringCL(sConsoleList[consoleNum].fontColour,str,len,sConsoleList[consoleNum].currentX,sConsoleList[consoleNum].currentY);
		sConsoleList[consoleNum].currentX = 0;
		sConsoleList[consoleNum].currentY += CHAR_HEIGHT;
	}

}

void sConsoleManageChar(char c) {

	static unsigned int currentCharPos = 0;
	unsigned char nSC;

	switch(c) {
		case '\t':
			nSC = nextSConsole();
		    uart_puts("NSC =  ");
		    uart_puts(uintToString(nSC,DECIMAL));
		    uart_puts("\n\r");

		    uart_puts("currentSConsole ");
		    uart_puts(uintToString(currentSConsole,DECIMAL));
		    uart_puts("\n\r");
			if(nSC == MAX_SCONSOLES) {
				drawBg();
			} else {
				focusSConsole(nSC);
			}
			break;
		case '-':
			if(currentSConsole < MAX_SCONSOLES) {
				currentCharPos--;
				eraseCharacterCL(sConsoleList[currentSConsole].backGroundColour, sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
				sConsoleList[currentSConsole].currentX -= CHAR_WIDTH;
				sConsoleList[currentSConsole].matrixMessages[sConsoleList[currentSConsole].messageCount][currentCharPos] = '\0';
				eraseCharacterCL(sConsoleList[currentSConsole].backGroundColour, sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
				drawCharacterCL(sConsoleList[currentSConsole].fontColour, '_', sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
			}
			break;
		case '+':
			if(currentSConsole < MAX_SCONSOLES) {
				if(sConsoleList[currentSConsole].messageCount == MAX_SCONSOLE_LINES) {
					sConsoleList[currentSConsole].messageCount = 0;
					currentCharPos = 0;
					clearSConsole(currentSConsole);
					sConsoleList[currentSConsole].currentX = 0;
					sConsoleList[currentSConsole].currentY = 3*CHAR_HEIGHT;
				}
				sConsoleList[currentSConsole].matrixMessages[sConsoleList[currentSConsole].messageCount][currentCharPos] = '\0';
				sConsoleList[currentSConsole].messageCount++;
				eraseCharacterCL(sConsoleList[currentSConsole].backGroundColour, sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
				sConsoleList[currentSConsole].currentX = 0;
				sConsoleList[currentSConsole].currentY += CHAR_HEIGHT;
				currentCharPos = 0;
				//executeCommand(sConsoleList[currentSConsole].matrixMessages[sConsoleList[currentSConsole].messageCount-1]);
				sConsoleWrite(currentSConsole, "NO WAITING PROCS EN ESTA CONSOLA!");
				drawStringCL(sConsoleList[currentSConsole].fontColour,sprompt,SPROMPT_LEN,sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
				sConsoleList[currentSConsole].currentX += SPROMPT_LEN*CHAR_WIDTH;
			}
			break;
		default:
			eraseCharacterCL(sConsoleList[currentSConsole].backGroundColour, sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
			drawCharacterCL(sConsoleList[currentSConsole].fontColour, (uint8_t) c, sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
			sConsoleList[currentSConsole].currentX += CHAR_WIDTH;
			sConsoleList[currentSConsole].matrixMessages[sConsoleList[currentSConsole].messageCount][currentCharPos] = c;
			drawCharacterCL(sConsoleList[currentSConsole].fontColour, '_', sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
			currentCharPos++;
	}

}

void sConsoleManageBlinkPrompt(void) {

	static unsigned char blink = 0;

	if(blink) {
		eraseCharacterCL(sConsoleList[currentSConsole].backGroundColour, sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
		blink = 0;
	} else {
		drawCharacterCL(sConsoleList[currentSConsole].fontColour, (uint8_t) '_', sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
		blink = 1;
	}
}
