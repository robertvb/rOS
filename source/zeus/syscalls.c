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

#include "../includes/zeus/syscalls.h"

static void SC_DO_NOTHING(void) {
	return;
}

/*
 * Entradas de la tabla de llamadas al sistema
 */
static system_call_entry_t system_call_table[] = {
	    {SC_EXIT,"exit",terminate_process,0,0},
	    {SC_UART_WRITE,"write",uart_puts,0,0},					// TODO
	    {SC_SLEEP,"sleep",sleepCurrentProc,0,0},				// TODO
	    {SC_GET_PID,"getpid", getCurrentProcessPid,0,0},
	    {SC_GET_PPID,"getppid", getCurrentProcessPpid,0,0},
		{SC_GET_CHAR,"getcharacter",getCharacterHandler,0,1}
};

static uint32_t MAX_SYSCALLS = 0;

void init_syscalls(void) {
	MAX_SYSCALLS = sizeof(system_call_table) / sizeof(system_call_table[0]);
}

/*
 * Rutina de manejo de interrupciones software
 */
int syscall_handler(unsigned int swi, unsigned int pc, unsigned int sp, unsigned int param0, unsigned int param1, unsigned int param2, unsigned int param3) {

    system_call_t* syscall;
    system_call_entry_t* syscall_entry;

	uart_puts("Handling syscall: ");
	uart_puts(system_call_table[swi].name);
	uart_puts("\n\r");

	if(swi >= MAX_SYSCALLS) {
		// SYSCALL no recenocida.
		uart_puts("[ERROR] SYSCALL NO RECONOCIDA!\n\r");
		return -1;
	}

    syscall_entry = &system_call_table[swi];
    syscall = ( system_call_t* )syscall_entry->function;

    // Llamamos a la rutina del kernel encargada de dicha llamada al sistema
    return syscall(pc,sp,param0,param1,param2,param3);

}
