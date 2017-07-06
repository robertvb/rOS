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

#ifndef PROCESS_H
#define	PROCESS_H

#include "../hades/rpi-uart.h"

/*
 * Estados del proceso
 */
#define PROCESS_STATUS_RUNNING		0	// En ejecucion
#define PROCESS_STATUS_READY		1	// Esperando su turno de ejecucion
#define PROCESS_STATUS_BLOCKED		2	// Esperando su turno en la cola de bloqueados
#define PROCESS_STATUS_ZOMBIE		3	// Muerto, pero esperando a algun hijo
#define PROCESS_STATUS_TERMINATED	4	// Terminado satisfactoriamente
#define PROCESS_STATUS_ABORTED		5	// Terminado abruptamente

/*
 * Motivos de bloqueo:
 * Los dos bits más significativos indican el motivo de bloqueo.
 * El resto se utilizan para calculos, flags, contadores, ej:
 * waiting_for = 0x00001000 -> el proceso esta en esperando pasivamente 4K tics de reloj
 */
#define BLKD_PASIVE_WAITING			0b00 // Espera pasiva wait()
#define BLKD_DISK_IO				0b01 // Esperando por el disco
#define BLKD_USER_IO				0b10 // Esperando por accion del usuario
#define BLKD_WAIT_PID				0b11 // Esperando a que termine proceso

#define GET_BLKD_REASON(WAITING_FOR) WAITING_FOR >> 30
#define GET_BLKD_ARGS(WAITING_FOR) WAITING_FOR & 0x3FFFFFFF

typedef unsigned int Pid_t;

typedef struct Process_t Process_t;

struct Process_t {
	Pid_t 			pid;			// Process ID
    Pid_t 			ppid;			// Parent Process ID
    char * 			name;			// Nombre
    char * 			fileName;		// Nombre de fichero
    unsigned char	terminal;		// Terminal asignado
    unsigned int	tablePageDir;	// Dirección tabla de paginas
    unsigned int 	stack_pointer;	// Puntero de pila
	unsigned int 	pc;				// contador de programa
	unsigned int	spsr;			// Registro de estado
    unsigned int 	times_loaded;	// Veces ejecutado
    unsigned int 	status;			// Estado
    unsigned int	waiting_for;	// Motivo de bloqueo + args
    Process_t * 	nextProc;		// Siguiente proceso de la cola
};

void sample_process_1();

void sample_process_2();

void sample_process_console();

#endif	/* PROCESS_H */

