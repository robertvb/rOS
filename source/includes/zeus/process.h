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

#include "../hades/rpi-uart.h"
#include "../atenea/pmem.h"

#ifndef PROCESS_H
#define	PROCESS_H

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
 * waiting_for = 0x00001000 -> el proceso esta en esperando pasivamente 4K microsegundos
 */
#define BLKD_PASIVE_WAITING			0
#define BLKD_DISK_IO				1
#define BLKD_USER_IO				2
#define BLKD_WAIT_PID				3

#define GET_BLKD_REASON(WAITING_FOR) WAITING_FOR >> 29
#define GET_BLKD_ARGS(WAITING_FOR) WAITING_FOR & 0x3FFFFFFF

typedef unsigned int Pid_t;

typedef struct Process_t Process_t;

struct Process_t {
	Pid_t 			pid;
    Pid_t 			ppid;
    char * 			name;
    char * 			fileName;
    unsigned int	tablePageDir;
    unsigned int 	stack_pointer;
	unsigned int 	pc;
    unsigned int 	times_loaded;
    unsigned int 	status;
    unsigned int	waiting_for;	// Recurso por el que se está esperando cuando el proceso esta bloqueado
    Process_t * 	nextProc;		// Siguiente proceso de la cola
};

void sample_process_1();

void sample_process_2();

#endif	/* PROCESS_H */

