#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

#define MAX_COMMANDS		50
#define MAX_SIZE_COMMAND	16
#define MAX_SIZE_COMMAND_DESCRP 61
#define MAX_SIZE_COMMAND_USAGE	31

typedef struct {

	char name[MAX_SIZE_COMMAND];				// Comando
	char descrp[MAX_SIZE_COMMAND_DESCRP];			// DEscripción
	char usage[MAX_SIZE_COMMAND_USAGE];			// Uso

	} command_t;


typedef struct {

	uint32_t currentX;					// En pixeles
	uint32_t currentY;					// En pixeles
	uint32_t maxX;						// En función de la resolución de la pantalla. En pixeles
	uint32_t maxY;						// En función de la resolución de la pantalla. En pixeles
	uint32_t lineSpacing;					// En pixeles (Interlineado)
	
	command_t commands[MAX_COMMANDS]; 			// Comandos disponibles
	uint8_t commandsNum;					// Numero real de comandos 
	} console_t;

#endif
