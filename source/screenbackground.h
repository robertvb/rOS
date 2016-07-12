#ifndef SCREEN_BACKGROUND_H
#define SCREEN_BACKGROUND_H

#include "rpi-systimer.h"
#include "rpi-atags.h"
#include "string.h"
#include "gpuFrameBuffer.h"
/*dsadsadsa*/

#define MAX_CONSOLE			4
#define MAX_SIZE_BOARD		40
#define VERTICAL_EDGES		CHAR_WIDTH		
#define HORIZONTAL_EDGES	CHAR_WIDTH
#define SHADOW_EDGES		2
#define LOGO_X				HORIZONTAL_EDGES +  8*CHAR_WIDTH
#define LOGO_Y				VERTICAL_EDGES   +  6*CHAR_WIDTH

typedef struct {
		uint32_t currentX;
		uint32_t currentY;
		uint32_t * onTime;
		uint8_t board[MAX_SIZE_BOARD];
		uint8_t rev[MAX_SIZE_BOARD];
		uint8_t serial[MAX_SIZE_BOARD];
		uint8_t mac[MAX_SIZE_BOARD];
		uint8_t freqCPU[MAX_SIZE_BOARD];
		uint8_t resolution[MAX_SIZE_BOARD];
		uint16_t bgColor;
		uint8_t workingChar;			// Guiño a miOS (/ - | - \) ;)
		uint8_t bwk;				// bool working char

	} bgInfo_t;

void bgInit(uint32_t atagsAddr);
void bgRefresh(void);
void bgWrite(uint8_t * string);
void bgDrawEdges(uint16_t color);
void bgDrawLogo(uint16_t color);
void parseAtags(uint32_t atags);
void displayBgInfo(void);

#endif
