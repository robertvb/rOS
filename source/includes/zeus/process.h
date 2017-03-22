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

#ifndef PROCESS_H
#define	PROCESS_H

#define PROCESS_STATUS_RUNNING		0	// En ejecucion
#define PROCESS_STATUS_WAITING		1	// Esperando su turno de ejecucion
#define PROCESS_STATUS_ZOMBIE		2	// Muerto, pero esperando a algun hijo
#define PROCESS_STATUS_TERMINATED	3	// Terminado satisfactoriamente
#define PROCESS_STATUS_ABORTED		4	// Terminado abruptamente

typedef unsigned int pid_t;

typedef struct {
	pid_t 			pid;
    char * 			name;
    pid_t 			ppid;
    unsigned int 	stack_pointer;
	unsigned int 	pc;
    unsigned int 	times_loaded;
    unsigned int 	status;
} process;

void sample_process_1();

void sample_process_2();

#endif	/* PROCESS_H */

