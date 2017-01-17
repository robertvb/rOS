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

#include "../includes/zeus/scheduler.h"

static waiting_queue_t waiting_queue;
static blocked_queue_t blocked_queue;
static struct proc_queue_node_t * current_process;
static uint32_t stacks[MAX_PROCS];

static void switch_context(void) {
    asm volatile("mov sp, %[addr]"
                 :: [addr]"r"((uint32_t)(current_process->proc.sp)));
    if (current_process->proc.rounds > 0)
    {
        asm volatile("pop {r0, lr}");
        asm volatile("add sp, sp, r0");
        asm volatile("pop {r0-r12}");
        asm volatile("rfeia sp!");
    }
    else
    {
        asm volatile("mov r0, %0" :: "r"(current_process->proc.pc));
        asm volatile("push {r0}");
        asm volatile("mov r0, %0" :: "r"(current_process->proc.r0));
        asm volatile("mov r1, %0" :: "r"(current_process->proc.r1));
        asm volatile("cpsie i"); // habilita interrupciones
        asm volatile("pop {pc}");
    }
}

static struct proc_queue_node_t * get_next_ready_proc(void) {

	uint32_t nbproc;
	nbproc = waiting_queue.counter;
	struct proc_queue_node_t *next_proc = NULL;
	/* vemos si hay algun proceso listo para ejecutarse */
	if(nbproc) {
		if(nbproc == 1) {
			next_proc = waiting_queue.firstProc;
			waiting_queue.firstProc = waiting_queue.lastProc = NULL;
		}
		else {
			next_proc = waiting_queue.firstProc;
			waiting_queue.firstProc = waiting_queue.firstProc->next;
		}
	}
	return next_proc;
}

/*
 * Add_proccess a la cola de preparados
 * Devuelve -1 si falla (memoria insuficiente)
 * o el pid del proceso.
 */
static pid_t add_process(const char *name, uint32_t pc, char **args, waiting_queue_t * queue) {

	struct proc_queue_node_t *proc_queue_node = kmalloc(sizeof(struct proc_queue_node_t));
    if (!proc_queue_node)
        return (pid_t) -1;

	proc_t* process = &proc_queue_node->proc;
    pid_t ppid = current_process != NULL ? current_process->proc.pid : -1;
    uint32_t sp = 0;
    uint32_t stack_index = 0;

    // añadimos a la cola de preparados, sera el siguiente en ejecutarse.

    proc_queue_node->next = queue->firstProc;
    queue->firstProc = proc_queue_node;

    // le asignamos una pila

    if (!stacks[0])
    {
        stacks[0] = _get_stack_pointer();
        sp = stacks[0];
        stack_index = 0;
    }
    else
    {
        int i;
        for (i = 0; i < MAX_PROCS; i++)
        {
            if (!stacks[i])
            {
                stacks[i] = stacks[0] + (i * STACK_SIZE);
                sp = stacks[i];
                stack_index = i;
                break;
            }
        }
        if (i >= MAX_PROCS)
        {
            kfree(process);
            return -1;
        }
    }

    pid_t pid = init_process(process,name,pc,ppid,args,sp,stack_index);
    queue->counter++;
    return pid;
}

void dispatch(void) {

    /* Save current process stack */
	if(current_process) {
		asm volatile("mov %0, r0\n\t" : "=r"(current_process->proc.sp));
		current_process->proc.rounds++;
	}
    /* check if hay alguno preparado para ejecutarse */
	struct proc_queue_node_t * next_proc = get_next_ready_proc();
    if(next_proc) {
    	/* Enviamos el proceso actual al final de la cola */
    	waiting_queue.lastProc->next = current_process;
    	waiting_queue.lastProc = current_process;
    	/* ejecutamos el nuevo */
    	current_process = next_proc;

    }
    /* cambio de contexto! */
    switch_context();

}

int init_scheduler(void) {

	/*
	 * Las colas de procesos bloqueados
	 * deben estar vacias en un primer momento.
	 */
	blocked_queue.firstProc = blocked_queue.lastProc = NULL;
	waiting_queue.firstProc = waiting_queue.lastProc = NULL;
	current_process = NULL;
	/*
	 * La cola de procesos preparados
	 * de momento contiene el proceso ocioso
	 */
	add_process("DummyProcess",(uint32_t) &loop,NULL,&waiting_queue);

	/* asignamos el last al primero */
	waiting_queue.lastProc = waiting_queue.firstProc;

	/* Para probar: */
	add_process("ProcesoA",(uint32_t) &processA,NULL,&waiting_queue);

	add_process("ProcesoB",(uint32_t) &processB,NULL,&waiting_queue);

	add_process("ProcesoC",(uint32_t) &processC,NULL,&waiting_queue);

	add_process("ProcesoD",(uint32_t) &processD,NULL,&waiting_queue);

	return 0;
}

void loop(void) {
	while(1)
		paintEntireScreen(White);
}

void processA() {
	while(1)
		paintEntireScreen(Red);
}

void processB() {
	while(1)
		paintEntireScreen(Blue);
}

void processC() {
	while(1)
		paintEntireScreen(Green);
}

void processD() {
	while(1)
		paintEntireScreen(Yellow);
}

