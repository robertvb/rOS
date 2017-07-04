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

static unsigned int getParameter(unsigned int sp, unsigned int param) {

	if(param > 10) {
		return 0;
	}
	return (unsigned int) (*(unsigned int *) (sp + param*4));
}

static void setParameter(unsigned int sp, unsigned int param, unsigned int value) {

	if(param > 10) {
		return;
	}

	*((unsigned int *) (sp + param*4)) = value;

}

static void SC_DO_NOTHING(void) {
	return;
}

static unsigned int wrapper_uart_write(unsigned int pc,unsigned int sp, unsigned int spsr) {
	unsigned int addr = getParameter(sp, 0);
	uart_puts((unsigned char *) addr);
	return sp;
}

static unsigned int wrapper_terminal_write(unsigned int pc,unsigned int sp, unsigned int spsr) {
	unsigned int addr = getParameter(sp, 0);
	sConsoleWrite(0,(unsigned char *) addr);
	return sp;
}

/*
 * Entradas de la tabla de llamadas al sistema
 */
static system_call_entry_t system_call_table[] = {
	    {SC_EXIT,"exit",(system_call_t) terminate_process,0,0},
	    {SC_UART_WRITE,"uart_write",wrapper_uart_write,0,1},
	    {SC_SLEEP,"sleep",sleepCurrentProc,0,1},
	    {SC_GET_PID,"getpid", getCurrentProcessPid,0,0},
	    {SC_GET_PPID,"getppid", getCurrentProcessPpid,0,0},
		{SC_GET_CHAR,"getcharacter",getCharacterHandler,0,0},
		{SC_TERMINAL_WRITE,"terminal_write",wrapper_terminal_write,0,1}
};

static uint32_t MAX_SYSCALLS = 0;

void init_syscalls(void) {
	MAX_SYSCALLS = sizeof(system_call_table) / sizeof(system_call_table[0]);
}

/*
 * Rutina de manejo de interrupciones software
 */
unsigned int syscall_handler(unsigned int sp_addr,unsigned int lr_addr,unsigned int spsr,unsigned int swi) {
    system_call_t syscall;
    system_call_entry_t* syscall_entry;
    unsigned int new_stack = 0;

	uart_puts("Handling syscall: ");
	uart_puts(uintToString(swi,DECIMAL));
	uart_puts(" with name ");
	uart_puts(system_call_table[swi].name);
	uart_puts("sp ");
	uart_puts(uintToString(sp_addr,HEXADECIMAL));
	uart_puts("\n\r");

	if(swi >= MAX_SYSCALLS) {
		// SYSCALL no recenocida.
		uart_puts("[ERROR] SYSCALL NO RECONOCIDA: ");
	    uart_puts(uintToString(swi,DECIMAL));
		uart_puts("\n\r");
		return 0;
	}

    syscall_entry = &system_call_table[swi];
    syscall = syscall_entry->function;

    // Llamamos a la rutina del kernel encargada de dicha llamada al sistema con el número de parámetros especificado.

    new_stack = syscall(lr_addr,sp_addr,spsr);

	uart_puts("new_stack: ");
    uart_puts(uintToString(new_stack,HEXADECIMAL));
	uart_puts("\n\r");

    return new_stack;
}
