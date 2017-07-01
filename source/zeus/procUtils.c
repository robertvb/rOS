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

#include "../includes/zeus/procUtils.h"

void init_queue(ProcessQueue_t * queue) {
	queue->head = queue->tail = NULL;
	return;
}

void addProcToQueue(Process_t * proc, ProcessQueue_t * queue) {

	/* cola vacia */
	if(queue->tail == NULL) {
		queue->head = queue->tail = proc;
		proc->nextProc = NULL;
	} else {
		queue->tail->nextProc = proc;
		queue->tail = proc;
		proc->nextProc = NULL;
	}
}
Process_t * getProcFromQueue(ProcessQueue_t * queue) {

	Process_t * proc = queue->head;

	/* caso cola vacia */
	if(queue->head != NULL) {
		queue->head = queue->head->nextProc;
	}

	return proc;

}
void addPidToQueue(Pid_t pid, ProcessQueue_t * queue) {

	Process_t * proc;

	proc = (Process_t *) getProcUsingPid(pid);

	/* cola vacia */
	if(queue->tail == NULL) {
		queue->head = queue->tail = proc;
		proc->nextProc = NULL;
	} else {
		queue->tail->nextProc = proc;
		queue->tail = proc;
		proc->nextProc = NULL;
	}

}
Pid_t getPidFromQueue(ProcessQueue_t * queue) {

	Process_t * proc = queue->head;

	/* caso cola vacia */
	if(queue->head != NULL) {
		queue->head = queue->head->nextProc;
	}

	return proc->pid;

}
