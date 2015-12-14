#include "gpuFrameBuffer.h"

extern FrameBufferInfo;
extern SystemDefaultFontInfo;

static rpi_gpu_framebuffer_descriptor_t * FrameBufferDescrp = (rpi_gpu_framebuffer_descriptor_t *) &FrameBufferInfo;
static rpi_gpu_mailbox_t * rpiGpuMailbox = (rpi_gpu_mailbox_t *) RPI_GPU_MAILBOX_BASE; 					//mailbox
static uint8_t * fontInfoAddress = (uint8_t *) &SystemDefaultFontInfo;

//FRAMEBUFFER METHODS


rpi_gpu_framebuffer_descriptor_t * RPI_GetFrameBufferDescpr(void) {

return (rpi_gpu_framebuffer_descriptor_t *) &FrameBufferInfo;

}

void paintEntireScreen(uint16_t Colour16b) {

	uint16_t * pixel = (uint16_t *) FrameBufferDescrp->pointer;
	uint32_t x,y;
	for(y = 0; y < FrameBufferDescrp->vHeight; y++)
		for(x = 0; x < FrameBufferDescrp->vWidth; x++)
			*(pixel++)= Colour16b;
}

void line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t Colour16b) {
	int32_t dx;
	int32_t sx;
	int32_t dy;
	int32_t sy;
	int32_t err;
	
	dx = (x1 > x0 ? x1-x0 : x0-x1); 
	sx = (x0 > x1 ? 1 : -1);
	dy = (y1 > y0 ? y1-y0 : y0-y1); 
 	sy = (y0 > y1 ? 1 : -1); 
	err= (dx > dy ? dx - dy : dy -dx);
	while((x0 != x1 + sx) && (y0 != y1 + sy)) {

		setPixel(x0,y0,Colour16b);

		if(err * 2 >= -dy) {
			
			x0 += sx;
			err-= dy;

		}

		if(err * 2 <= dx) {

			y0 += sy;
			err+= dx;

		}

	}
						
}

void setPixel(int32_t posX, int32_t posY, uint16_t Colour16b) {

		
	uint16_t *ptr;     
	uint32_t  offset;
	if(posX < FrameBufferDescrp->vWidth && posY < FrameBufferDescrp->vHeight) {
		
		        
		        offset = (posY * FrameBufferDescrp->pitch) + (posX << 1);
			ptr = (uint16_t *) (FrameBufferDescrp->pointer + offset);
			*ptr = Colour16b;
	
	}
}

/* Devuelve el caracer que ha pintado*/
int8_t drawCharacter(uint8_t Char, uint32_t x, uint32_t y) {

	uint8_t bit;
	uint8_t readedBits;
	uint8_t row;
	uint8_t * currentCharAddress;

	if(Char > 127)	//vemos si el caracter esta dentro de los 128 que podemos representar
		return -1;
	
	currentCharAddress=(fontInfoAddress + Char * 16);

	for(row = 0; row < CHAR_HEIGHT; row++) {
	
		readedBits = *(currentCharAddress + row);

		for(bit = 0; bit < CHAR_WIDTH; bit ++) {
		
			if(((readedBits >> bit) & 0x1) != 0)
				setPixel(x + bit, y + row,GreenYellow);

		}

	}
	return Char;

}

int8_t drawString(uint8_t * string, uint32_t length, uint32_t x, uint32_t y) {

	uint32_t x0 = x;
	uint32_t x1;
	uint8_t nextChar;
	if(length == 0)
		return 0;

	for(nextChar = 0; nextChar < length -1 && string[nextChar]; nextChar++) {

		switch(string[nextChar]) {

			case '\n':
				x = x0;
				y += CHAR_HEIGHT;
				break;

			case '\t':
				break;

			default:
				drawCharacter(string[nextChar],x,y);
				x += CHAR_WIDTH;

		}
	}

	return nextChar + 1;
}
