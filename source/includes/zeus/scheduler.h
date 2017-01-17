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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include "../hades/gpuFrameBuffer.h"

#define MAX_PROCS 1024

/*
 * Nodo de cola de procesos
 */
struct proc_queue_node_t {
	struct proc_t proc;
	struct proc_queue_node_t * next;
};

/*
 * Colas de procesos preparados y bloqueados
 */
typedef struct proc_queue_t {
	struct proc_queue_node_t * firstProc;
	struct proc_queue_node_t * lastProc;
	uint32_t counter;
} waiting_queue_t, blocked_queue_t;

int init_scheduler(void);
void dispatch(void);

/* TEST */

void processA(void);
void processB(void);
void processC(void);
void processD(void);
void loop(void);

#endif /* SOURCE_INCLUDES_ZEUS_SCHEDULER_H_ */
