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

static Process_t process_list[MAX_PROCS];
static Process_t * ready_queue;
static Process_t * ready_queue_tail;
static Process_t * bloqued_queue;
static Process_t * active_process;
static unsigned int stack_base;
static unsigned int process_count;
// TODO DEBUG
static unsigned int count;

static void mainProcLoop() {
	while(1) {
		uart_puts("Bucle del proceso main!\r\n");
	}
}

Pid_t getNextPid(void) {
	return process_count;
}

Process_t * getCurrentProcess(void) {
	return active_process;
}

Process_t * stepReadyQueue(void) {
	Process_t * proc = ready_queue;
	ready_queue = ready_queue->nextProc;
	active_process = proc;
	active_process->nextProc = NULL;
	active_process->status = PROCESS_STATUS_RUNNING;
	return proc;
}

void addToReadyQueue(Process_t * proc) {
	if(ready_queue == NULL) {
		ready_queue = ready_queue_tail = proc;
	} else {
		ready_queue_tail->nextProc = proc;
		ready_queue_tail = proc;
		proc->nextProc = NULL;
	}
	proc->status = PROCESS_STATUS_READY;
}

void setActiveProcess(Process_t * proc) {
	active_process = proc;
	proc->nextProc = NULL;
}

Pid_t getCurrentProcessPid(void) {
	return active_process->pid;
}

Pid_t getCurrentProcessPpid(void) {
	return active_process->ppid;
}

unsigned char * getCurrentProcessTerminal(void) {
	return active_process->terminal;
}

Process_t * getProcUsingPid(Pid_t pid) {
	return (Process_t *) &process_list[pid];
}

unsigned int terminate_process(void) {
	// Cambiamos el estado a terminado, de esta manera el scheduler lo ignorará
	active_process->status = PROCESS_STATUS_TERMINATED;

	// desbloqueamos la consola del proceso
	unLockSConsole(active_process->terminal);

	if(ready_queue == NULL) {
		uart_puts("Se ha terminado el unico proceso que estaba en la cola. Proc que vamos a poner a ejecutar: ");
		active_process = &process_list[0];
	} else {
		active_process = ready_queue;
		ready_queue = ready_queue->nextProc;
	}

	active_process->nextProc = NULL;

    // Incremetamos estadisticas y cambiamos status a RUNNING
    active_process->times_loaded++;
    active_process->status = PROCESS_STATUS_RUNNING;

    uart_puts("Restoring stack 0x");
    uart_puts(uintToString(active_process->stack_pointer,HEXADECIMAL));
	uart_puts("\n\r");

    uart_puts("Restoring pc 0x");
    uart_puts(uintToString(active_process->pc,HEXADECIMAL));
	uart_puts("\n\r");

    return active_process->stack_pointer;

}

// Funcion para crear el proceso base.
void init_proc_scheduling() {

    process_count = 0;
    process_list[process_count].pid = process_count;
    process_list[process_count].ppid = process_count;
    process_list[process_count].name = "Main";
    // El proceso simplemente ejecuta un while true
    process_list[process_count].pc = (unsigned int) &mainProcLoop;
    process_list[process_count].spsr = 0x10;
    process_list[process_count].times_loaded = 1;
    process_list[process_count].status = PROCESS_STATUS_READY;
    process_list[process_count].tablePageDir = NULL; 	// El proceso main no tiene tabla de paginas.
    process_list[process_count].nextProc = NULL;		// En este punto no hay otros procesos en la cola
    process_list[process_count].terminal = 5;
    bloqued_queue = NULL;					// El proceso main es el primero en ejecutarse

    // Se obtiene la dirección actual de la pila
    unsigned int stack_pointer = 0x401f3c;

	// Se utiliza como dirección base del puntero de pila del proceso
	stack_base = stack_pointer;

	// Output DEBUG
	uart_puts("Main stack is 0x");
    uart_puts(uintToString(stack_pointer,HEXADECIMAL));
	uart_puts("\n\r");

    // Se establece como proceso activo en este momento
    ready_queue = ready_queue_tail = NULL;
    process_list[process_count].nextProc = NULL;
    active_process = &process_list[process_count];

    // Se incrementa el contador de procesos

    process_count++;
    // TODO DEBUG
    count = 0;
}

/*
 * Procedure to fork this process, creating a new one, pointing the pc
 * to the memory address of the desired procedure
 */

void  kInternalExecute(char * name,  Dir_t pc) {

	// Comprobación si se ha alcanzado el número máximo de procesos
	if(process_count >= MAX_PROCS) {
		uart_puts("ERROR! ALCANZADO MAX_PROCS");
		return;
	}

	// Obtención del PID
	Pid_t pid = getNextPid();

	// Obtencion de un marco de 4k para espacio del proceso
	unsigned int frameDir = get4kframe(pid);

	if(frameDir == NULL) {
		uart_puts("ERROR! NO SE PUEDE RESERVAR MEMORIA PARA EJECUTAR EL PROCESO");
		return;
	}

	// Calculamos dirección de la PILA
	Dir_t forked_stack_pointer = (Dir_t) frameDir + PROC_FRAME_SIZE;

	// DEBUG
	uart_puts("Forked stack is 0x");
    uart_puts(uintToString((unsigned int) forked_stack_pointer, HEXADECIMAL));
	uart_puts("\n\r");

	process_list[process_count].pid = pid;
	process_list[process_count].name = (char *) name;
	process_list[process_count].pc = (unsigned int) pc;
	process_list[process_count].ppid = active_process->ppid;
	process_list[process_count].times_loaded = 0;
	process_list[process_count].status = PROCESS_STATUS_READY;
	process_list[process_count].tablePageDir = getNextFld(pid);    //TODO TEST
	process_list[process_count].spsr = 0x00000150;			  //Modo usuario
	process_list[process_count].terminal = getCurrentSConsole();


    forked_stack_pointer--;
    *forked_stack_pointer = get4kframe(pid);
    uart_puts("Poniendo a cero word = ");
    uart_puts(uintToString((unsigned int) forked_stack_pointer,HEXADECIMAL));
    uart_puts("\n\r");

    forked_stack_pointer--;
    *forked_stack_pointer = (unsigned int) pc;
    uart_puts("Poniendo a cero word = ");
    uart_puts(uintToString((unsigned int) forked_stack_pointer,HEXADECIMAL));
    uart_puts("\n\r");


    unsigned int i = 14;
    while( i-- > 0) {
        forked_stack_pointer--;
    	*forked_stack_pointer = 0;
        uart_puts("Poniendo a cero word = ");
        uart_puts(uintToString((unsigned int) forked_stack_pointer,HEXADECIMAL));
        uart_puts("\n\r");
    }

	process_list[process_count].stack_pointer = forked_stack_pointer;

	if(ready_queue == NULL) {
		ready_queue_tail = &process_list[process_count];
		process_list[process_count].nextProc = NULL;
	} else {
		process_list[process_count].nextProc = ready_queue;
	}

	ready_queue = &process_list[process_count];

    process_count++;

}

/*
 * Just keep the processor busy
 */
void halt() {
	uart_puts("Halting!\n\r");
	while(1);
}

/*
 * Procedimiento entrante desde IRQ interrupt,
 * Cambia al siguiente proceso
 */
unsigned int schedule_timeout(unsigned int stack_pointer, unsigned int pc, unsigned int spsr) {

	unsigned int consolaAct = getCurrentSConsole();
	if(getDebugTracerLever() == 3) {
		debugPrintStrV3("Se ha producido una INTERRUPCION DE RELOJ. Se invoca rutina 'schedule_timeout' del modulo 'zeus'\n\r");
		debugPrintStrV3("ID del proceso interrumpido: '");
	    uart_puts(uintToString(active_process->pid,DECIMAL));
	    uart_puts("' with name '");
	    uart_puts(active_process->name);
	    uart_puts("'\n\r");
		debugPrintStrV3("CONSOLA ACT:  ");
		debugPrintStrV3(uintToString(consolaAct,DECIMAL));
		uart_puts("\n\r");
	}

	if(consolaAct == 5) {
		debugPrintStrV3("actualizando BackGround.\n\r");
		bgRefresh();
	}

	unsigned int new_stack;
   //uart_puts("ejecutando time_out. Count = ");
   //uart_puts(uintToString(count++,DECIMAL));
   //uart_puts("\n\r");
	// Se salva PC y STACK del proceso en ejecucion
	active_process->stack_pointer = stack_pointer;
    active_process->pc = pc;
    active_process->spsr = spsr;

    // Se actualiza su status a READY y se pone al final de la cola
	active_process->status = PROCESS_STATUS_READY;

    Process_t * proc;
    debugPrintStrV3("estado de la readyQ: \n\r");
    if(getDebugTracerLever() == 3) {
		for(proc = ready_queue; proc != NULL; proc = proc->nextProc) {
			debugPrintStrV3("Encontrado un proceso en la cola de preparados. Pid = ");
			uart_puts(uintToString(proc->pid,DECIMAL));
			if(ready_queue_tail == proc) {
				uart_puts(" y es last executed");
			}
			uart_puts("\n\r");
		}
    }

    // Actuamos la cola de bloqueados
    //Process_t * proc;
    Process_t * procAnt;

    debugPrintStrV3("Actuando sobre la cola de bloqueados: \n\r");
    for(procAnt = proc = bloqued_queue; proc != NULL;) {
    	debugPrintStrV3("encontrado un proc en bloqueados. Pid: ");
    	if(getDebugTracerLever() == 3) {
			uart_puts(uintToString(proc->pid,DECIMAL));
			uart_puts("\n\r");
    	}
        unsigned int reason = GET_BLKD_REASON(proc->waiting_for);
    	if(BLKD_PASIVE_WAITING == GET_BLKD_REASON(proc->waiting_for)) {
    		unsigned int newValue = GET_BLKD_ARGS(proc->waiting_for);
    		newValue--;
    		debugPrintStrV3("Numero de TICS restantes para desbloqueo = '");
        	if(getDebugTracerLever() == 3) {
				uart_puts(uintToString(newValue,DECIMAL));
				uart_puts("'\n\r");
        	}
    		if(newValue == 0) {
    			// lo metemos en la cola de preparados
            	if(getDebugTracerLever() == 3) {
    			debugPrintStrV3("DESBLOQUEADO PID = ");
					uart_puts(uintToString(proc->pid,DECIMAL));
					uart_puts("\n\r");
            	}
	            // actualizamos cola bloqueados
                if(bloqued_queue == procAnt) {
                	bloqued_queue = proc->nextProc;
                }
                procAnt = proc->nextProc;
            	// fin act cola bloqueados
	            if(ready_queue == NULL) {
	            	// si esta vacia
		            ready_queue_tail = proc;
					ready_queue = ready_queue_tail;

	            } else {
		            ready_queue_tail ->nextProc = proc;
		            // actualizamos cola bloqueados
	            	// fin act cola bloqueados
	            }
	            ready_queue_tail = proc;
    	    	proc = proc->nextProc;
    	    	ready_queue_tail->nextProc = NULL;
    		} else {
    			proc->waiting_for = newValue;
    	    	procAnt = proc;
    	    	proc = proc->nextProc;
    		}
    	} else {
	    	procAnt = proc;
	    	proc = proc->nextProc;
    	}
    }
    //uart_puts("Done! \n\r");

    // DEBUG CODE
	if(getDebugTracerLever() == 3) {
		debugPrintStrV3("Guardando SP = '0x");
		uart_puts(uintToString(stack_pointer,HEXADECIMAL));
		uart_puts("'\n\r");

		debugPrintStrV3("Guardando PC = '0x");
		uart_puts(uintToString(pc,HEXADECIMAL));
		uart_puts("'\n\r");

		debugPrintStrV3("Guardando SPSR = '0x");
		uart_puts(uintToString(spsr,HEXADECIMAL));
		uart_puts("'\n\r");
	}

	// Obtenemos siguiente proceso y encolamos el actual.

	// Caso en el que no hay procesos en la cola (se está ejecutando el ocioso únicamente).
	if(ready_queue != NULL ) {
		if(ready_queue->pid == 0) {
			if(ready_queue == ready_queue_tail) {
				// Caso en el que el procesos ocioso es el único en la lista. Continuamos ejecutando el actual
				//uart_puts("SOLO PROC OCIOSO EN COLA, SE IGNORA.\n\r");
			} else {
				// Caso en el que nos encontramos al proceso ocioso y hay mas procesos: nos lo saltamos
				ready_queue_tail->nextProc = active_process;
				active_process->nextProc = ready_queue;
				ready_queue_tail = ready_queue;
				active_process = ready_queue->nextProc;
				ready_queue = ready_queue->nextProc->nextProc;
			}
		} else {
			ready_queue_tail->nextProc = active_process;
			ready_queue_tail = active_process;
			active_process = ready_queue;
			ready_queue = ready_queue->nextProc;
		}
	}

	active_process->nextProc = NULL;

    // Incremetamos estadisticas y cambiamos status a RUNNING
    active_process->times_loaded++;
    active_process->status = PROCESS_STATUS_RUNNING;

	if(getDebugTracerLever() == 3) {

		debugPrintStrV3("ID del siguiente proceso a ejecutar: '");
		uart_puts(uintToString(active_process->pid,DECIMAL));
		uart_puts("con nombre '");
		uart_puts(active_process->name);
		uart_puts("'\n\r");

		debugPrintStrV3("Restaurando SP = '0x");
		uart_puts(uintToString(active_process->stack_pointer,HEXADECIMAL));
		uart_puts("'\n\r");

		debugPrintStrV3("Restaurando PC = '0x");
		uart_puts(uintToString(active_process->pc,HEXADECIMAL));
		uart_puts("'\n\r");

		debugPrintStrV3("Restaurando SPSR = '0x");
		uart_puts(uintToString(active_process->spsr,HEXADECIMAL));
		uart_puts("'\n\r");
	}

	return active_process->stack_pointer;

}

unsigned int sleepCurrentProc(unsigned int addr, unsigned int sp, unsigned int spsr, unsigned int tics) {


	uart_puts("metiendo proceso en la cola de bloqueados. Pid is ");
    uart_puts(uintToString(active_process->pid,DECIMAL));
    uart_puts(" con sp =  ");
    uart_puts(uintToString(sp,HEXADECIMAL));
    uart_puts(" con addrlr = ");
    uart_puts(uintToString(addr,HEXADECIMAL));
    uart_puts(" con los siguientes tics: ");
    uart_puts(uintToString(tics,DECIMAL));
    uart_puts("\r\n");

	active_process->status = PROCESS_STATUS_BLOCKED;
	active_process->waiting_for = 0x0FFFFFFF & tics;

	// Se salva PC y STACK del proceso en ejecucion
	active_process->stack_pointer = sp;
    active_process->pc = addr;
    active_process->spsr = spsr;

	uart_puts("Valor waiting_for: ");
    uart_puts(uintToString(active_process->waiting_for,DECIMAL));
    uart_puts("\r\n");

	if(bloqued_queue == NULL) {
		uart_puts("la cola estaba vacia!\n\r");
		bloqued_queue = active_process;
		active_process->nextProc = NULL;
	} else {
		uart_puts("la cola NO estaba vacia: \n\r");
		active_process->nextProc = bloqued_queue;
		bloqued_queue = active_process;
	}

    uart_puts("Estado de la cola de bloqueados tras añadir el proceso: \n\r");
	Process_t * proc;
	for(proc = bloqued_queue; proc != NULL; proc = proc->nextProc) {
        uart_puts("ENCONTRADO PID = ");
        uart_puts(uintToString(proc->pid,DECIMAL));
        uart_puts("\n\r");
	}

	if(ready_queue == NULL) {
		uart_puts("has bloqeuado al unico proceso en cola. Proc que vamos a poner a ejecutar: ");
		active_process = &process_list[0];
	} else {
		active_process = ready_queue;
		ready_queue = ready_queue->nextProc;
	}

	active_process -> nextProc = NULL;

	uart_puts("Proc que vamos a poner a ejecutar: ");
    uart_puts(uintToString(active_process->pid,DECIMAL));
    uart_puts("\r\n");

	return active_process->stack_pointer;
}

unsigned int uart_interrupt_handler(unsigned int stack_pointer, unsigned int pc) {

	// Se salva PC y STACK del proceso en ejecucion
	active_process->stack_pointer = stack_pointer;
	active_process->pc = pc;

	uart_puts("Handling uart int! ");
	uart_puts("\n\r");

	uart_puts("tecla: '");
	unsigned int uCharacter = (*((unsigned int *) UART0_DR));
	uart_putc(uCharacter);
	uart_puts("'\n\r");

	sConsoleManageChar((unsigned char) uCharacter);

	Process_t * proc;
	Process_t * lastProc;
	// TODO DEBUG
	uart_puts("Actuando sobre la cola de bloqueados: \n\r");
	for (lastProc = proc = bloqued_queue;
			proc != NULL && GET_BLKD_REASON(proc->waiting_for) != BLKD_USER_IO; lastProc= proc, proc = proc->nextProc);

	if (proc == NULL) {
		uart_puts("No hay proceso esperando por uart input \n\r");
	} else {
		uart_puts("Encontrado proceso bloqueado esperando uart. Pid = ");
		uart_puts(uintToString(proc->pid, DECIMAL));
		uart_puts("\n\r");

		setParameter(proc->stack_pointer,0,uCharacter);

		if (lastProc == proc) {
			bloqued_queue = proc->nextProc;
		} else {
			lastProc->nextProc = proc->nextProc;
		}

	}

	// si habia un proc bloqueado esperando un caracer lo ponemos a ejecutar
	if(proc != NULL) {

		if(ready_queue == NULL) {
			ready_queue = ready_queue_tail = active_process;
		} else {
			ready_queue_tail->nextProc = active_process;
			ready_queue_tail = active_process;
		}

		active_process = proc;
	}

	// A PARTIR DE AQUI COPIADO DEL DISPATCHER

	// Incremetamos estadisticas y cambiamos status a RUNNING
	active_process->times_loaded++;
	active_process->status = PROCESS_STATUS_RUNNING;

	uart_puts("stack saved, was 0x");
	uart_puts(uintToString(stack_pointer, HEXADECIMAL));
	uart_puts("\n\r");

	uart_puts("Saving pc...");
	uart_puts(uintToString(pc, HEXADECIMAL));
	uart_puts("\n\r");

	// Incremetamos estadisticas y cambiamos status a RUNNING
	active_process->times_loaded++;
	active_process->status = PROCESS_STATUS_RUNNING;

	uart_puts("Restoring stack 0x");
	uart_puts(uintToString(active_process->stack_pointer, HEXADECIMAL));
	uart_puts("\n\r");

	uart_puts("Restoring pc 0x");
	uart_puts(uintToString(active_process->pc, HEXADECIMAL));
	uart_puts("\n\r");

	return active_process->stack_pointer;

}

unsigned int getCharacterHandler(unsigned int pc, unsigned int sp, unsigned int spsr) {

	active_process->status = PROCESS_STATUS_BLOCKED;
	active_process->waiting_for = (BLKD_USER_IO << 30);
	// Se salva PC y STACK del proceso en ejecucion
	active_process->stack_pointer = sp;
    active_process->pc = pc;
    active_process->spsr = spsr;

    // Se bloquea al proceso solicitante hasta que este disponible el recurso
    active_process->nextProc = bloqued_queue;
    bloqued_queue = active_process;

	uart_puts("<<<<<<<<<<<<<<<<<<ESPERANDO CARACTER>>>>>>>>>>>>>>>>>>>\r\n");

	// TODO PRINTF DE DEBUG
	if (ready_queue == NULL) {
		uart_puts("REady Q = NULL");
	} else {
		uart_puts("REady Q = ");
		uart_puts(uintToString(ready_queue->pid, DECIMAL));
		uart_puts("\n\r");
	}

    // Ejecutar siguiente proceso. COPIADO DE SLEEPCURRENTPROC

	if(ready_queue == NULL) {
		uart_puts("has bloqeuado al unico proceso en cola. Proc que vamos a poner a ejecutar: ");
		active_process = &process_list[0];
	} else {
		active_process = ready_queue;
		ready_queue = ready_queue->nextProc;
	}

	active_process -> nextProc = NULL;

	return active_process->stack_pointer;

}
