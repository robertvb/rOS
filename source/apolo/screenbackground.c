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


#include "../includes/apolo/screenbackground.h"

/*
 * De esta manera hubicamos BackGroundInfo en la sección data definida en boot.S
 * en vez de en la sección .bss (discarded)
 */
extern BackgroundInfo;
static bgInfo_t * bgInfo = (bgInfo_t *) &BackgroundInfo;

void bgInit(uint32_t atagsAddr) {


	
	bgInfo->bgColor = 0x2844;
	bgInfo->workingChar = '|';
	bgInfo->bwk = 0;
	bgInfo->onTime = 0;
	RPI_GetSystemTimer()->counter_lo = 0;

	/************* Empezamos a pintar el BackGround con la info obtenida ************/

	paintEntireScreen(bgInfo->bgColor);

	bgDrawEdges(LightGrey);

	bgDrawLogo(GreenYellow);

	parseAtags(atagsAddr);

	RPI_GetSystemTimer()->counter_lo = 0;
	displayBgInfo();

	bgInfo->currentY=240;

}

void bgRefresh(void) {

	displayBgInfo();
}

void bgDrawEdges(uint16_t color) {
	
	uint32_t i,j;
	rpi_gpu_framebuffer_descriptor_t * FrameBufferDescrp = (rpi_gpu_framebuffer_descriptor_t *)  RPI_GetFrameBufferDescpr();

	/*borde superior*/

	for(j = 0; j < HORIZONTAL_EDGES - 1; j++)
		for(i = 0; i < FrameBufferDescrp->vWidth - 1; i ++)
			setPixel(i,j,color);

	/*Detalle en negro del borde superior */

	for(j = HORIZONTAL_EDGES - 1; j < HORIZONTAL_EDGES + SHADOW_EDGES - 1; j ++)
		for(i = VERTICAL_EDGES; i < FrameBufferDescrp->vWidth - VERTICAL_EDGES; i ++)
			setPixel(i,j,Black);

	/* Borde inferior */

	for(j = FrameBufferDescrp->vHeight - HORIZONTAL_EDGES - 1; j < FrameBufferDescrp->vHeight - 1; j++)
		for(i = 0; i < FrameBufferDescrp->vWidth - 1; i ++)
			setPixel(i,j,color);

	/* Detalle en negro del borde inferior */

	for(j =  FrameBufferDescrp->vHeight - HORIZONTAL_EDGES - SHADOW_EDGES - 1; j < FrameBufferDescrp->vHeight - HORIZONTAL_EDGES - 1; j ++)
		for(i = VERTICAL_EDGES - 1; i < FrameBufferDescrp->vWidth - VERTICAL_EDGES - 1; i++)
			setPixel(i,j,Black);

	/* Borde izquierdo */

	for(j = HORIZONTAL_EDGES - 1; j < FrameBufferDescrp->vHeight - HORIZONTAL_EDGES - 1; j++)
		for(i = 0; i < VERTICAL_EDGES - SHADOW_EDGES - 1; i++)
			setPixel(i,j,color);
	
	/* Datalle en negro del borde izquierdo */

	for(j = HORIZONTAL_EDGES - 1; j < FrameBufferDescrp->vHeight - HORIZONTAL_EDGES - SHADOW_EDGES - 1; j++)
		for(i = HORIZONTAL_EDGES - 1; i < HORIZONTAL_EDGES + SHADOW_EDGES -1; i++)
			setPixel(i,j,Black);

	/* Borde derecho */

	for(j = HORIZONTAL_EDGES - 1; j < FrameBufferDescrp->vHeight - HORIZONTAL_EDGES - 1; j++)
		for(i = FrameBufferDescrp->vWidth - VERTICAL_EDGES - 1; i < FrameBufferDescrp->vWidth - 1; i ++)
			setPixel(i,j,color);
			
	/* Detalle en negro del borde derecho */

	for(j = HORIZONTAL_EDGES + SHADOW_EDGES - 1; j < FrameBufferDescrp->vHeight - HORIZONTAL_EDGES - SHADOW_EDGES - 1; j++)
		for(i = FrameBufferDescrp->vWidth - VERTICAL_EDGES - SHADOW_EDGES - 1; i < FrameBufferDescrp->vWidth -VERTICAL_EDGES - 1; i ++)
			setPixel(i,j,Black);
}

void bgDrawLogo(uint16_t color) {

	bgInfo->currentX = LOGO_X;
	bgInfo->currentY = LOGO_Y;

	drawString("___  _ ____ _  _ _  _ ____ _  _ _ ___  ____    ____ \0",80,bgInfo->currentX,bgInfo->currentY);
	bgInfo->currentY+=CHAR_HEIGHT;
	drawString("|__] | |___ |  | |  | |___ |  | | |    |  |    |__| \0",80,bgInfo->currentX,bgInfo->currentY);
	drawCharacter(92,bgInfo->currentX+13*CHAR_WIDTH,bgInfo->currentY);
	drawCharacter(92,bgInfo->currentX+28*CHAR_WIDTH,bgInfo->currentY);
	drawCharacter(92,bgInfo->currentX+37*CHAR_WIDTH,bgInfo->currentY);
	bgInfo->currentY+=CHAR_HEIGHT;
	drawString("|__] | |___ |  |   /  |___ |  | | |__/ |__|    |  |  \0",80,bgInfo->currentX,bgInfo->currentY);
	drawCharacter(92,bgInfo->currentX+18*CHAR_WIDTH,bgInfo->currentY);
	drawCharacter(92,bgInfo->currentX+14*CHAR_WIDTH,bgInfo->currentY);
	drawCharacter(92,bgInfo->currentX+29*CHAR_WIDTH,bgInfo->currentY);

	
	bgInfo->currentY+=2*CHAR_HEIGHT;
	drawString("____ ____ ____ \0",80,bgInfo->currentX,bgInfo->currentY);
	bgInfo->currentY+=CHAR_HEIGHT;
	drawString("|__/ |  | [__ \0",80,bgInfo->currentX,bgInfo->currentY);
	bgInfo->currentY+=CHAR_HEIGHT;
	drawString("|    |__| ___].  v0.2.\0",80,bgInfo->currentX,bgInfo->currentY);
	drawCharacter(92,bgInfo->currentX+3*CHAR_WIDTH,bgInfo->currentY);


}

void parseAtags(uint32_t atagsAddr) {

	struct atag_header *atags = (struct atag_header *) atagsAddr;
	uint32_t tag;
	uint8_t * cmdLine;
	uint32_t pos;
	do
	{
		tag = atags->tag;

		switch(tag)
		{
			case 0:
				break;
			case ATAG_CORE:
				break;
			case ATAG_MEM:
			//	bgInfo->memSize		 = ((struct atag_mem *)atags)->size;
			//	bgInfo->memStartAddress  = ((struct atag_mem *)atags)->address;
				break;
			case ATAG_VIDEOTEXT:
				break;
			case ATAG_RAMDISK:
				break;
			case ATAG_INITRD2:
				break;
			case ATAG_SERIAL:
				break;
			case ATAG_REVISION:
				break;
			case ATAG_VIDEOLFB:
				break;
			case ATAG_CMDLINE:
				cmdLine = &(((struct atag_cmdline *)atags)->cmdline);		// todos salvados menos cmdline
				break;
			default:
				return;
		}

		atags = (struct atag_header *)((uint32_t)atags + (atags->size * 4));
	} while(tag);

	/* Ahora buscamos los patrones que queremos mostrar en el bg */

	if(searchStringPattern("bcm2708", cmdLine)) {
	
		strncpy(bgInfo->board,"bcm2708",40);

		pos = searchStringPattern("boardrev=",cmdLine);

		if(pos) {

			strncpy(bgInfo->rev,cmdLine+pos,4);
		}
		else { // revision de la placa desconocida, es una RPI, pero no sabemos cual. something is something!
			
			strncpy(bgInfo->rev,"desconocida",40);
		}

		pos = searchStringPattern("clock_freq=",cmdLine);

		if(pos) {

			strncpy(bgInfo->freqCPU,cmdLine+pos,10);

		}
		else {

			strncpy(bgInfo->freqCPU,"desconocida",40);

		}

		pos = searchStringPattern("serial=",cmdLine);

		if(pos) {

			strncpy(bgInfo->serial,cmdLine+pos,11);
		}
		else {

			strncpy(bgInfo->serial,"desconocido",40);
		}

		pos = searchStringPattern("macaddr=",cmdLine);

		if(pos) {

			strncpy(bgInfo->mac,cmdLine + pos,18);
		}
		else {
		
			strncpy(bgInfo->mac,"desconocida",40);
		}

	}
	else // placa base desconocida. Malo malo, estamos en algo que no es una RPI :(
		strncpy(bgInfo->board,"desconocida. Buena suerte amigo!",40);
}

void displayBgInfo(void) {


	uint16_t currentX;
	uint16_t currentX2;
	uint16_t x0;
	uint16_t currentY;
	rpi_gpu_framebuffer_descriptor_t * FrameBufferDescrp;

	FrameBufferDescrp = (rpi_gpu_framebuffer_descriptor_t *) RPI_GetFrameBufferDescpr();
	x0 = currentX	  = FrameBufferDescrp->vWidth - FrameBufferDescrp->vWidth/3;
	currentY 	  = FrameBufferDescrp->vHeight/2;

	currentX += drawString("SISTEMA OPERATIVO : ",100,currentX,currentY+=CHAR_HEIGHT)*CHAR_WIDTH/2;
	drawString("Estructura: Monolitico",100,currentX,currentY+=2*CHAR_HEIGHT);
	drawString("Long. de palabra: 32 bits",100,currentX,currentY+=CHAR_HEIGHT);
	currentX2 +=currentX + drawString("Tiempo encendido:",100,currentX,currentY+=CHAR_HEIGHT)*CHAR_WIDTH;

	char  onTimeMinute [] = {"            \0"};
	char  onTimeSecond [] = {"            \0"};
	int actualMsecond = RPI_GetSystemTimer()->counter_lo;

	if(actualMsecond >= bgInfo->onTime * 1000000) {

		bgInfo->onTime += 1;
		 uintToStringStr(bgInfo->onTime / 60,DECIMAL,onTimeMinute);
		strncat(onTimeMinute, "m",12);
		 uintToStringStr(bgInfo->onTime % 60,DECIMAL,onTimeSecond);
		strncat(onTimeSecond,"s",12);
		eraseString(bgInfo->bgColor,currentX2,currentY,10);
		currentX2 += drawString(onTimeMinute,12,currentX2,currentY)*CHAR_WIDTH;
		currentX2 += drawString(onTimeSecond,12,currentX2,currentY)*CHAR_WIDTH;
	}

	drawString("HARDWARE SUBYACENTE : ", 100,x0,currentY+=2*CHAR_HEIGHT)*CHAR_WIDTH;
	currentX2 = currentX + drawString("Placa base: ",100,currentX,currentY+=2*CHAR_HEIGHT)*CHAR_WIDTH;
	if(bgInfo->board[0] != '\0')
		drawString(bgInfo->board,40,currentX2,currentY);
	else{
		drawString("ERROR",40,currentX2,currentY);
	}
	currentX2 = currentX + drawString("Revision: ",100,currentX,currentY+=CHAR_HEIGHT)*CHAR_WIDTH;
	drawString(bgInfo->rev,40,currentX2,currentY);
	currentX2 = currentX + drawString("Num. serial: ",100,currentX,currentY+=CHAR_HEIGHT)*CHAR_WIDTH;
	drawString(bgInfo->serial,40,currentX2,currentY)*CHAR_WIDTH;
	currentX2 = currentX + drawString("MAC: ",100,currentX,currentY+=CHAR_HEIGHT)*CHAR_WIDTH;
	drawString(bgInfo->mac,40,currentX2,currentY)*CHAR_WIDTH;
	currentX2 = currentX + drawString("CPU's: 1",100,currentX,currentY+=CHAR_HEIGHT)*CHAR_WIDTH;
	currentX2 = currentX + drawString("Frecuencia CPU: ",100,currentX,currentY+=CHAR_HEIGHT)*CHAR_WIDTH;
	drawString(bgInfo->freqCPU,40,currentX2,currentY)*CHAR_WIDTH;

	drawString("PANTALLA : ",100,x0,currentY+=2*CHAR_HEIGHT)*CHAR_WIDTH;
	drawString("Resolucion: 1024 x 768 ",100,currentX,currentY+=2*CHAR_HEIGHT);
	

}
