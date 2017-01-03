#ifndef SCREEN_BACKGROUND_H
#define SCREEN_BACKGROUND_H

#include "../hades/rpi-systimer.h"
#include "../hades/rpi-atags.h"
#include "../hefesto/string.h"
#include "../hades/gpuFrameBuffer.h"

#define MAX_CONSOLE		4
#define MAX_SIZE_BOARD		40
#define VERTICAL_EDGES		CHAR_WIDTH		
#define HORIZONTAL_EDGES	CHAR_WIDTH
#define SHADOW_EDGES		2
#define LOGO_X			HORIZONTAL_EDGES +  8*CHAR_WIDTH
#define LOGO_Y			VERTICAL_EDGES   +  6*CHAR_WIDTH

typedef struct {
		uint32_t currentX;						// Posición actual X
		uint32_t currentY;						// Posición actual Y
		uint32_t onTime;						// Tiempo que lleva el sistema encendido
		uint8_t board[MAX_SIZE_BOARD];			// Nombre de la placa (familia)
		uint8_t rev[MAX_SIZE_BOARD];			// Revisión
		uint8_t serial[MAX_SIZE_BOARD];			// Num serial de la placa (Hexadecimal)
		uint8_t mac[MAX_SIZE_BOARD];			// Dir Mac de la placa base
		uint8_t freqCPU[MAX_SIZE_BOARD];		// Freq. de la CPU (250000000 sin overClock)
		uint8_t resolution[MAX_SIZE_BOARD];		// Resolucion de pantalla
		uint16_t bgColor;						// Color del fondo
		uint8_t workingChar;					// Guiño a miOS (/ - | - \) ;)
		uint8_t bwk;							// bool working char
	} bgInfo_t;

void bgInit(uint32_t atagsAddr);
void bgRefresh(void);
void bgWrite(uint8_t * string);
void bgDrawEdges(uint16_t color);
void bgDrawLogo(uint16_t color);
void parseAtags(uint32_t atags);
void displayBgInfo(void);

#endif
