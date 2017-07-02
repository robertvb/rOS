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

static ScreenConsole_t sConsoleList[MAX_SCONSOLES];
static SCid_t currentSConsole;
static unsigned int currentCharPos;

void init_screen_consoles(void) {

	currentSConsole = MAX_SCONSOLES; 	/* bgInfo */

	char i;
	for(i = 0; i < MAX_SCONSOLES; i++) {
		sConsoleList[i].backGroundColour = Black;
		sConsoleList[i].fontColour = White;
		sConsoleList[i].currentX = 0;
		sConsoleList[i].currentY = 3*CHAR_HEIGHT;
		sConsoleList[i].messageCount = 0;
		sConsoleList[i].bloquedProcs.head = NULL;
		sConsoleList[i].bloquedProcs.tail = NULL;
		clearSConsole(i);
	}

	currentCharPos = 0;

}

unsigned char getCurrentSConsole(void) {
	return currentSConsole;
}

static SCid_t nextSConsole(void) {
	return currentSConsole = ((currentSConsole + 1) % (MAX_SCONSOLES + 1));
}

void setSConsoleFontColour(SCid_t consoleNum, uint16_t colour) {
	sConsoleList[consoleNum].fontColour = colour;
}

void setSConsolebackGroundColour(SCid_t consoleNum, uint16_t colour) {
	sConsoleList[consoleNum].backGroundColour = colour;
}

void clearSConsole(SCid_t consoleNum) {

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
		for(y = 0; y < frameBufferDscr->vHeight; y++) {
			for(x = 0; x < frameBufferDscr->vWidth; x++) {
				if(y <= CHAR_HEIGHT || y >= frameBufferDscr->vHeight - CHAR_HEIGHT) {
					*(pixel++) = LightGrey;
				} else {
					*(pixel++) = sConsoleList[consoleNum].backGroundColour;
				}
			}
		}

		drawStringCL(Black,"Tab = Next Terminal | '+' = Enter | '-' = Delete | Help para ayuda",68,20*CHAR_WIDTH,frameBufferDscr->vHeight - CHAR_HEIGHT);

		sConsoleList[consoleNum].currentX += drawStringCL(Black," rOS v0.8 | Terminal ",20,0,0);
		drawStringCL(Black,uintToString(currentSConsole,DECIMAL),2,21*CHAR_WIDTH,0);

		sConsoleList[consoleNum].currentX = 0;
		sConsoleList[consoleNum].currentY = 3*CHAR_HEIGHT;

		drawStringCL(sConsoleList[currentSConsole].fontColour,sprompt,SPROMPT_LEN,sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
		strncpy(sConsoleList[currentSConsole].matrixMessages[sConsoleList[currentSConsole].messageCount],sprompt,SPROMPT_LEN);
		currentCharPos+= SPROMPT_LEN;
		sConsoleList[currentSConsole].currentX += SPROMPT_LEN*CHAR_WIDTH;

	}

}

void focusSConsole(SCid_t consoleNum) {

	if(consoleNum >= MAX_SCONSOLES) {
		return;
	}

	sConsoleList[consoleNum].currentY = 3*CHAR_HEIGHT;

	rpi_gpu_framebuffer_descriptor_t * frameBufferDscr  = (rpi_gpu_framebuffer_descriptor_t*) RPI_GetFrameBufferDescpr();
	uint16_t * pixel = (uint16_t *) frameBufferDscr->pointer;
	uint32_t x,y;
	for(y = 0; y < frameBufferDscr->vHeight; y++) {
		for(x = 0; x < frameBufferDscr->vWidth; x++) {
			if(y <= CHAR_HEIGHT || y >= frameBufferDscr->vHeight - CHAR_HEIGHT) {
				*(pixel++) = LightGrey;
			} else {
				*(pixel++) = sConsoleList[consoleNum].backGroundColour;
			}
		}
	}

	drawStringCL(Black,"Tab = Next Terminal | '+' = Enter | '-' = Delete | Help para ayuda",68,20*CHAR_WIDTH,frameBufferDscr->vHeight - CHAR_HEIGHT);

	sConsoleList[consoleNum].currentX += drawStringCL(Black," rOS v0.8 | Terminal ",20,0,0);
	drawStringCL(Black,uintToString(currentSConsole,DECIMAL),2,21*CHAR_WIDTH,0);

	sConsoleList[consoleNum].currentX = CHAR_WIDTH;

	for(x = 0; x < sConsoleList[consoleNum].messageCount; x++) {
		unsigned char* str = sConsoleList[consoleNum].matrixMessages[x];
		unsigned int len = stringLength(str);
		if(len > 0) {
			drawStringCL(sConsoleList[consoleNum].fontColour,str,len+1,sConsoleList[consoleNum].currentX,sConsoleList[consoleNum].currentY);
			sConsoleList[consoleNum].currentY += CHAR_HEIGHT;
			sConsoleList[consoleNum].currentX = CHAR_WIDTH;
		}
	}


	drawStringCL(sConsoleList[currentSConsole].fontColour,sprompt,SPROMPT_LEN,sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
	strncpy(sConsoleList[currentSConsole].matrixMessages[sConsoleList[currentSConsole].messageCount],sprompt,SPROMPT_LEN);
	currentCharPos+= SPROMPT_LEN - 1;
	sConsoleList[currentSConsole].currentX += SPROMPT_LEN*CHAR_WIDTH;

}

void sConsoleWrite(SCid_t consoleNum, char * str) {

	if(consoleNum >= MAX_SCONSOLES) {
		return;
	}

	const unsigned int len = stringLength(str);

	if(len > MAX_SCONSOLE_LINE_LEN) {
		return;
	}

	if(sConsoleList[currentSConsole].messageCount == MAX_SCONSOLE_LINES) {
		sConsoleList[currentSConsole].messageCount = 0;
		currentCharPos = 0;
		clearSConsole(currentSConsole);
		sConsoleList[currentSConsole].currentX = 0;
		sConsoleList[currentSConsole].currentY = 3*CHAR_HEIGHT;
	}

	/* para pintarlo despues del prompt. efecto visual */
	sConsoleList[consoleNum].messageCount++;
	strncpy(sConsoleList[consoleNum].matrixMessages[sConsoleList[consoleNum].messageCount],"      ",SPROMPT_LEN);
	strncpy(sConsoleList[consoleNum].matrixMessages[sConsoleList[consoleNum].messageCount]+SPROMPT_LEN - 1,(uint8_t *)str,len+1);

	if(currentSConsole == consoleNum) {
		sConsoleList[consoleNum].currentX+= SPROMPT_LEN * CHAR_WIDTH;
		drawStringCL(sConsoleList[consoleNum].fontColour,str,len+1,sConsoleList[consoleNum].currentX,sConsoleList[consoleNum].currentY);
		sConsoleList[consoleNum].currentX = 0;
		sConsoleList[consoleNum].currentY += CHAR_HEIGHT;
	}

}

void sConsoleManageChar(char c) {

	unsigned char nSC;

	switch(c) {
		case '\t':
			nSC = nextSConsole();
			sConsoleList[currentSConsole].matrixMessages[sConsoleList[currentSConsole].messageCount][0] = '\0';
			if(nSC == MAX_SCONSOLES) {
				drawBg();
			} else {
				focusSConsole(nSC);
			}
			break;
		case '-':
			if(currentSConsole < MAX_SCONSOLES && currentCharPos > SPROMPT_LEN - 1) {
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
				eraseCharacterCL(sConsoleList[currentSConsole].backGroundColour, sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
				sConsoleList[currentSConsole].currentX = 0;
				sConsoleList[currentSConsole].currentY += CHAR_HEIGHT;
				currentCharPos = 0;
				uart_puts("Se va a ejecutar: ");
				uart_puts(sConsoleList[currentSConsole].matrixMessages[sConsoleList[currentSConsole].messageCount]);
				uart_puts("\n\r");
				executeCommand(sConsoleList[currentSConsole].matrixMessages[sConsoleList[currentSConsole].messageCount] + SPROMPT_LEN - 1);
				sConsoleList[currentSConsole].messageCount++;
				drawStringCL(sConsoleList[currentSConsole].fontColour,sprompt,SPROMPT_LEN,sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
				strncpy(sConsoleList[currentSConsole].matrixMessages[sConsoleList[currentSConsole].messageCount],sprompt,SPROMPT_LEN);
				currentCharPos+= SPROMPT_LEN - 1;
				sConsoleList[currentSConsole].currentX += SPROMPT_LEN*CHAR_WIDTH;
			}
			break;
		default:
			if(sConsoleList[currentSConsole].bloquedProcs.head == NULL) {
				/* si no hay procesos bloqueados asociados a este terminal */
				eraseCharacterCL(sConsoleList[currentSConsole].backGroundColour, sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
				drawCharacterCL(sConsoleList[currentSConsole].fontColour, (uint8_t) c, sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
				sConsoleList[currentSConsole].currentX += CHAR_WIDTH;
				sConsoleList[currentSConsole].matrixMessages[sConsoleList[currentSConsole].messageCount][currentCharPos] = c;
				drawCharacterCL(sConsoleList[currentSConsole].fontColour, '_', sConsoleList[currentSConsole].currentX,sConsoleList[currentSConsole].currentY);
				currentCharPos++;
			} else {
				/* se envia el caracter a dicho proceso */
			}
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
