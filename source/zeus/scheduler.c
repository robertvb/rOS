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
static Process_t * last_executed;
static Process_t * bloqued_queue;
static Process_t * active_process;
static unsigned int stack_base;
static unsigned int process_count;

extern void timer_reset();

static void mainProcLoop() {
	uart_puts("Bucle del proceso main!\r\n");
}

Pid_t getNextPid(void) {
	return process_count;
}

Pid_t getCurrentProcessPid(void) {
	return active_process->pid;
}

Pid_t getCurrentProcessPpid(void) {
	return active_process->ppid;
}

void terminate_process() {
	// Cambiamos el estado a terminado, de esta manera el scheduler lo ignorará
	active_process->status = PROCESS_STATUS_TERMINATED;

    // Obtenemos el siguiente proceso
	active_process = ready_queue;

    // If -1, halt
    if (active_process == NULL) {
		uart_puts("No more waiting processes, halting.");
		uart_puts("\n\r");
		halt();
	}

    // Incremetamos estadisticas y cambiamos status a RUNNING
    active_process->times_loaded++;
    active_process->status = PROCESS_STATUS_RUNNING;

    // Actualizacion del puntero de pila en el procesador al nuevo proceso a ejecutar
	asm volatile("MOV SP, %[addr]" : : [addr] "r" ((unsigned int )(active_process->stack_pointer)) );

	// Si no es la primera vez que se ejecuta el proceso
	if (active_process->times_loaded > 1) {

		timer_reset();

		// Rescatamos registros y flags de la pila
		asm volatile("pop {R0}");
		/* Saved Program Status Register
		 * Upon taking an exception, the CPSR is copied to the SPSR of the processor
		 * mode the exception is taken to.
		   This is useful because the exception handler is able to restore the CPSR
		   to the value prior to taking the exception,
		   as well as being able to examine the CPSR in general.
		   MSR = MOVE SYSTEM REGYSTER
		 */
		asm volatile("MSR   SPSR_cxsf, R0");
		asm volatile("pop {LR}");
		asm volatile("pop {R0 - R12}");

		/* Rehabilitacion de interrupciones
		 * CPS change procesor state IE interrupt and i ENABLE
		 */
		asm volatile("cpsie i");

		// Rescatamos el contador de programa y renaudamos la ejecucion
		asm volatile("pop {PC}");

	} else {

		// Se salva el contador de programa en la pila
		unsigned int addr = (unsigned int )(active_process->pc);
		asm volatile("MOV R0, %[addr]" : : [addr] "r" (addr) );
		asm volatile("push {R0}");

		timer_reset();

		/* Rehabilitacion de interrupciones
		 * CPS change procesor state IE interrupt and i ENABLE
		 */
		asm volatile("cpsie i");

		/* rescatamos PC y comienza la ejecucion del proceso */
		asm volatile("pop {PC}");

	}

}

// Funcion para crear el proceso base.
// El equivalente en UNIX sería el proceso INIT
void create_main_process() {

    process_count = 0;
    process_list[process_count].pid = process_count;
    process_list[process_count].name = "Main";
    // El proceso simplemente ejecuta un while true
    process_list[process_count].pc = (unsigned int) &halt;
    process_list[process_count].times_loaded = 1;
    process_list[process_count].status = PROCESS_STATUS_ZOMBIE;
    process_list[process_count].tablePageDir = NULL; 	// El proceso main no tiene tabla de paginas.
    process_list[process_count].nextProc = NULL;		// En este punto no hay otros procesos en la cola
    ready_queue = bloqued_queue = NULL;					// El proceso main es el primero en ejecutarse

    // Se obtiene la dirección actual de la pila
    unsigned int stack_pointer;
	asm volatile ("MOV %0, SP\n\t" : "=r" (stack_pointer) );

	// Se utiliza como dirección base del puntero de pila del proceso
	stack_base = stack_pointer;

	// Output DEBUG
	uart_puts("Main stack is 0x");
    uart_puts(uintToString(stack_pointer,HEXADECIMAL));
	uart_puts("\n\r");

    // Se establece como proceso activo en este momento
    active_process = &process_list[process_count];

    // Se incrementa el contador de procesos
    process_count++;
}

/*
 * Procedure to fork this process, creating a new one, pointing the pc
 * to the memory address of the desired procedure
 */
void kfork(char * name, Dir_t pc, Dir_t forked_stack_pointer) {

	if(process_count >= MAX_PROCS) {
		uart_puts("ERROR! ALCANZADO MAX_PROCS");
		return;
	}

    process_count++;

	// DEBUG
	uart_puts("Forked stack is 0x");
    uart_puts(uintToString((unsigned int) forked_stack_pointer,HEXADECIMAL));
	uart_puts("\n\r");

	process_list[process_count].pid = process_count;
	process_list[process_count].name = name;
	process_list[process_count].pc = (unsigned int) pc;
	process_list[process_count].ppid = active_process->ppid;
	process_list[process_count].times_loaded = 0;
	process_list[process_count].stack_pointer = (unsigned int) forked_stack_pointer;
	process_list[process_count].status = PROCESS_STATUS_READY;
	process_list[process_count].tablePageDir = 0x1f500000;    //TODO TEST

	process_list[process_count].nextProc = active_process;
    ready_queue = &process_list[process_count];
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
void schedule_timeout(unsigned int stack_pointer, unsigned int pc) {

	// Se salva PC y STACK del proceso en ejecucion
	active_process->stack_pointer = stack_pointer;
    active_process->pc = pc;

    // Se actualiza su status a READY y se pone al final de la cola
    if (active_process->status == PROCESS_STATUS_RUNNING) {
		active_process->status = PROCESS_STATUS_READY;

		// Colocamos el proceso actual al final de la cola de preparados
		last_executed->nextProc = active_process;
		last_executed = active_process;
		active_process-> nextProc = NULL;
	}

    // Actuamos la cola de bloqueados
    Process_t * proc;
    Process_t * procAnt;
    uart_puts("Actuando sobre la cola de bloqueados: \n\r");
    for(procAnt = proc = bloqued_queue; proc != NULL; proc = proc->nextProc) {
        uart_puts("encontrado un proc en bloqueados! \n\r");
        unsigned int reason = GET_BLKD_REASON(proc->waiting_for);
    	if(BLKD_PASIVE_WAITING == GET_BLKD_REASON(proc->waiting_for)) {
    		unsigned int newValue = GET_BLKD_ARGS(proc->waiting_for);
    		newValue--;
    		if(newValue == 0) {
    			if(bloqued_queue == procAnt) {
    				bloqued_queue = procAnt = proc->nextProc;
    			} else {
    				procAnt->nextProc = proc->nextProc;
    			}
    			// lo metemos en la cola de preparados
    			last_executed->nextProc = proc;
    			proc->nextProc = NULL;
    		} else {
    			proc->waiting_for = (reason << 29) & newValue;
    	    	procAnt = proc;
    		}
    	}
    }

    uart_puts("Done! \n\r");

    // DEBUG CODE
    uart_puts("\n");
	uart_puts("\n\r");
    uart_puts("Schedule timeout. Current active pid is ");
    uart_puts(uintToString(active_process->pid,DECIMAL));
    uart_puts(" with name ");
    uart_puts(active_process->name);
    uart_puts(". Switching to next process.\n\r");

    uart_puts("stack saved, was 0x");
    uart_puts(uintToString(stack_pointer,HEXADECIMAL));
	uart_puts("\n\r");

	uart_puts("Saving pc...");
    uart_puts(uintToString(pc,HEXADECIMAL));
	uart_puts("\n\r");

    // Obtenemos el siguiente proceso
	active_process = ready_queue;

    // If -1, halt
    if (active_process == NULL) {
		uart_puts("No more waiting processes, halting.");
		uart_puts("\n\r");
		halt();
	}

    // Actualizamos la cola
    ready_queue = ready_queue->nextProc;

    // Incremetamos estadisticas y cambiamos status a RUNNING
    active_process->times_loaded++;
    active_process->status = PROCESS_STATUS_RUNNING;

    uart_puts("Restoring stack 0x");
    uart_puts(uintToString(active_process->stack_pointer,HEXADECIMAL));
	uart_puts("\n\r");

    uart_puts("Restoring pc 0x");
    uart_puts(uintToString(active_process->pc,HEXADECIMAL));
	uart_puts("\n\r");

    // Actualizacion del puntero de pila en el procesador al nuevo proceso a ejecutar
	asm volatile("MOV SP, %[addr]" : : [addr] "r" ((unsigned int )(active_process->stack_pointer)) );

	// Si no es la primera vez que se ejecuta el proceso
	if (active_process->times_loaded > 1) {

		/* refresco del cacheo de la TLB y reasignacion de la tabla de paginas */
		unsigned int pagetable = active_process->tablePageDir;
		/* Use translation table 0 up to 64MB */
		asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (6));
		/* Translation table 0 - ARM1176JZF-S manual, 3-57 */
		asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pagetable));
		/* Invalidate the translation lookaside buffer (TLB)
		 * ARM1176JZF-S manual, p. 3-86
		 */
		asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
		/* end*/

		timer_reset();

		// Rescatamos registros y flags de la pila
		asm volatile("pop {R0}");
		/* Saved Program Status Register
		 * Upon taking an exception, the CPSR is copied to the SPSR of the processor
		 * mode the exception is taken to.
		   This is useful because the exception handler is able to restore the CPSR
		   to the value prior to taking the exception,
		   as well as being able to examine the CPSR in general.
		   MSR = MOVE SYSTEM REGYSTER
		 */
		asm volatile("MSR   SPSR_cxsf, R0");
		asm volatile("pop {LR}");
		asm volatile("pop {R0 - R12}");

		/* Rehabilitacion de interrupciones
		 * CPS change procesor state IE interrupt and i ENABLE
		 */
		asm volatile("cpsie i");

		// Rescatamos el contador de programa y renaudamos la ejecucion
		asm volatile("pop {PC}");

	} else {

		// Se salva el contador de programa en la pila
		unsigned int addr = (unsigned int )(active_process->pc);
		asm volatile("MOV R0, %[addr]" : : [addr] "r" (addr) );
		asm volatile("push {R0}");

		/* refresco del cacheo de la TLB y reasignacion de la tabla de paginas */
		unsigned int pagetable = active_process->tablePageDir;
		/* Use translation table 0 up to 64MB */
		asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (6));
		/* Translation table 0 - ARM1176JZF-S manual, 3-57 */
		asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pagetable));
		/* Invalidate the translation lookaside buffer (TLB)
		 * ARM1176JZF-S manual, p. 3-86
		 */
		asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));

		/* end*/

		timer_reset();

		/* Rehabilitacion de interrupciones
		 * CPS change procesor state IE interrupt and i ENABLE
		 */

		asm volatile("cpsie i");

		/* rescatamos PC y comienza la ejecucion del proceso */
		asm volatile("pop {PC}");

	}

}

void sleepCurrentProc(unsigned int tics) {
	active_process->status = PROCESS_STATUS_BLOCKED;
	active_process->waiting_for = 0x0FFFFFFF & tics;

	//TODO
}

