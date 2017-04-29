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

static process process_list[1024];
static unsigned int stack_base;
static unsigned int process_count;
static unsigned int active_process_index;

extern void _main_endloop();
extern void timer_reset();

Pid_t getNextPid(void) {
	return process_count;
}

void terminate_process() {
	// Cambiamos el estado a terminado, de esta manera el scheduler lo ignorará
	process_list[active_process_index].status = PROCESS_STATUS_TERMINATED;

	// ejecutamos el siguiente proceso.

    int next_process = next_waiting_process_index();

    // If -1, halt
    if (next_process < 0) {
		uart_puts("No more waiting processes, halting.");
		uart_puts("\n\r");
		halt();
	}

    // Actualizamos al siguiente proceso en ejecucion
    active_process_index = next_process;

    // Incremetamos estadisticas y cambiamos status a RUNNING
    process_list[active_process_index].times_loaded++;
    process_list[active_process_index].status = PROCESS_STATUS_RUNNING;

    // Actualizacion del puntero de pila en el procesador al nuevo proceso a ejecutar
	asm volatile("MOV SP, %[addr]" : : [addr] "r" ((unsigned int )(process_list[active_process_index].stack_pointer)) );

	// Si no es la primera vez que se ejecuta el proceso
	if (process_list[active_process_index].times_loaded > 1) {

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
		unsigned int addr = (unsigned int )(process_list[active_process_index].pc);
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

    process main_process;

    process_count = 0;
    main_process.pid = process_count;
    main_process.name = "Main";
    // El proceso simplemente ejecuta un while true
    main_process.pc = (unsigned int) &_main_endloop;
    main_process.times_loaded = 1;
    main_process.status = PROCESS_STATUS_ZOMBIE;

    // Se obtiene la dirección actual de la pila
    unsigned int stack_pointer;
	asm volatile ("MOV %0, SP\n\t" : "=r" (stack_pointer) );

	// Se utiliza como dirección base del puntero de pila del proceso
	stack_base = stack_pointer;

	// Output DEBUG
	uart_puts("Main stack is 0x");
    uart_puts(uintToString(stack_pointer,HEXADECIMAL));
	uart_puts("\n\r");

    // Se guarda el proceso en la tabla de procesos
    process_list[process_count] = main_process;

    // Se establece como proceso activo en este momento
    active_process_index = process_count;

    // Se incrementa el contador de procesos
    process_count++;
}

/*
 * Procedure to fork this process, creating a new one, pointing the pc
 * to the memory address of the desired procedure
 */
void kfork(char * name, Dir_t pc) {

    process fork_process;

	// Basic memory organization to get new stack addr.
    // Se añade 1024 bytes al anterior stack_base
	unsigned int * forked_stack_pointer = (unsigned int *) stack_base + (process_count * 1024);

	// DEBUG
	uart_puts("Forked stack is 0x");
    uart_puts(uintToString((unsigned int) forked_stack_pointer,HEXADECIMAL));
	uart_puts("\n\r");

    fork_process.pid = process_count;
    fork_process.name = name;
    fork_process.pc = (unsigned int) pc;
    fork_process.ppid = active_process_index;
    fork_process.times_loaded = 0;
    fork_process.stack_pointer = (unsigned int) forked_stack_pointer;
    fork_process.status = PROCESS_STATUS_WAITING;
    fork_process.tablePageDir = 0x1f500000;    //TODO TEST

    process_list[process_count] = fork_process;

    process_count++;
}

/*
 * Rutina de obtencion el siguiente proceso preparado
 */
int next_waiting_process_index() {

	// Start in the active index
	int next_process_index = active_process_index;

	// Do this while the actual process isnt in the waiting status and not reach the actual running process
	// Obtenemos el siguiente proceso con status WAITING, si se llega de nuevo al proceso actual, se ejecuta otra vez.
	do {
		next_process_index++;

		// rewind the list
		if (next_process_index == process_count) {
			next_process_index = 0;
		}

	} while ((process_list[next_process_index].status != PROCESS_STATUS_WAITING) && (next_process_index != active_process_index));

	// If the found process isnt waiting
	if (process_list[next_process_index].status != PROCESS_STATUS_WAITING) {
		return -1;
	}

	return next_process_index;
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
    process_list[active_process_index].stack_pointer = stack_pointer;
    process_list[active_process_index].pc = pc;

    // Se actualiza su status a WAITING
    if (process_list[active_process_index].status == PROCESS_STATUS_RUNNING) {
		process_list[active_process_index].status = PROCESS_STATUS_WAITING;
	}

    // DEBUG CODE
    uart_puts("\n");
	uart_puts("\n\r");
    uart_puts("Schedule timeout. Current active pid is ");
    uart_puts(uintToString(process_list[active_process_index].pid,DECIMAL));
    uart_puts(" with name ");
    uart_puts(process_list[active_process_index].name);
    uart_puts(". Switching to next process.\n\r");

    uart_puts("stack saved, was 0x");
    uart_puts(uintToString(stack_pointer,HEXADECIMAL));
	uart_puts("\n\r");

	uart_puts("Saving pc...");
    uart_puts(uintToString(pc,HEXADECIMAL));
	uart_puts("\n\r");

    // Obtenemos el siguiente proceso
    int next_process = next_waiting_process_index();

    // If -1, halt
    if (next_process < 0) {
		uart_puts("No more waiting processes, halting.");
		uart_puts("\n\r");
		halt();
	}

    // Actualizamos al siguiente proceso en ejecucion
    active_process_index = next_process;

    // Incremetamos estadisticas y cambiamos status a RUNNING
    process_list[active_process_index].times_loaded++;
    process_list[active_process_index].status = PROCESS_STATUS_RUNNING;

    uart_puts("Restoring stack 0x");
    uart_puts(uintToString(stack_pointer,HEXADECIMAL));
	uart_puts("\n\r");

    uart_puts("Restoring pc 0x");
    uart_puts(uintToString(process_list[active_process_index].pc,HEXADECIMAL));
	uart_puts("\n\r");

    // Actualizacion del puntero de pila en el procesador al nuevo proceso a ejecutar
	asm volatile("MOV SP, %[addr]" : : [addr] "r" ((unsigned int )(process_list[active_process_index].stack_pointer)) );

	// Si no es la primera vez que se ejecuta el proceso
	if (process_list[active_process_index].times_loaded > 1) {

		/* refresco del cacheo de la TLB y reasignacion de la tabla de paginas */
		invalidate_TLB0();
		unsigned int pagetable = process_list[active_process_index].tablePageDir;
		asm volatile("mcr p15, 0, %[pagetable], c1, c0, 0" : : [pagetable] "r" (pagetable));
	    uart_puts("setting PAGETABLE to: ");
	    uart_puts(uintToString(pagetable,HEXADECIMAL));
		uart_puts("\n\r");
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
		unsigned int addr = (unsigned int )(process_list[active_process_index].pc);
		asm volatile("MOV R0, %[addr]" : : [addr] "r" (addr) );
		asm volatile("push {R0}");

		/* refresco del cacheo de la TLB y reasignacion de la tabla de paginas */
		invalidate_TLB0();
		unsigned int pagetable = process_list[active_process_index].tablePageDir;
		asm volatile("mcr p15, 0, %[pagetable], c1, c0, 0" : : [pagetable] "r" (pagetable));
	    uart_puts("setting PAGETABLE to: ");
	    uart_puts(uintToString(pagetable,HEXADECIMAL));
		uart_puts("\n\r");
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

