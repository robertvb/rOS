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

unsigned int getParameter(unsigned int sp, unsigned int param) {

	if(param > 10) {
		return 0;
	}
	return (unsigned int) (*(unsigned int *) (sp + param*4));
}

void setParameter(unsigned int sp, unsigned int param, unsigned int value) {

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
	sConsoleWrite(getCurrentProcessTerminal(),(unsigned char *) addr);
	return sp;
}

static unsigned int wrapper_sleepCurrentProc(unsigned int pc,unsigned int sp, unsigned int spsr) {
	unsigned int tics = getParameter(sp, 0);
	return sleepCurrentProc(pc,sp,spsr,tics);
}

static unsigned int wrapper_getCurrentProcessPid(unsigned int pc,unsigned int sp, unsigned int spsr) {
	unsigned int pid = getCurrentProcessPid();
	setParameter(sp,0,pid);
	return sp;
}

static unsigned int wrapper_writeMailBox(unsigned int pc,unsigned int sp, unsigned int spsr) {
	unsigned int nBuz = getParameter(sp, 0);
	unsigned int messageDir = getParameter(sp, 1);
	writeMessage(nBuz,messageDir);
	return sp;
}

static unsigned int wrapper_readMailBox(unsigned int pc,unsigned int sp, unsigned int spsr) {
	unsigned int nBuz = getParameter(sp, 0);
	unsigned int messageDir = getParameter(sp, 1);
	sp = readMessage(nBuz,messageDir);
	return sp;
}

/*
 * Entradas de la tabla de llamadas al sistema
 */
static system_call_entry_t system_call_table[] = {
	    {SC_EXIT,"exit",(system_call_t) terminate_process,0,0},
	    {SC_UART_WRITE,"uart_write",wrapper_uart_write,0,1},
	    {SC_SLEEP,"sleep",wrapper_sleepCurrentProc,0,1},
	    {SC_GET_PID,"getpid", wrapper_getCurrentProcessPid,0,0},
	    {SC_GET_PPID,"getppid", getCurrentProcessPpid,0,0},
		{SC_GET_CHAR,"getcharacter",getCharacterHandler,0,0},
		{SC_TERMINAL_WRITE,"terminal_write",wrapper_terminal_write,0,1},
		{SC_EXEC_PROC,"exec_process",kInternalExecute,0,1},
		{SC_READ_MAILBOX,"read_mailBox",wrapper_writeMailBox,0,2},
		{SC_WRITE_MAILBOX,"write_mailBox",wrapper_readMailBox,0,2}
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

    debugPrintStrV1("Se ha producido una LLAMADA AL SISTEMA. Se invoca rutina 'syscall_handler' del modulo 'zeus'\n\r");
    debugPrintStrV1("ID de la llamada solicitada: '");
    debugPrintStrV1(uintToString(swi,DECIMAL));
    uart_puts("' con nombre '");
    debugPrintStrV1(system_call_table[swi].name);
    uart_puts("'\n\r");
    debugPrintStrV1("ID del proceso solicitante: '");
    debugPrintStrV1(uintToString(getCurrentProcess()->pid,DECIMAL));
    uart_puts("' con nombre '");
    debugPrintStrV1(getCurrentProcess()->name);
    debugPrintStrV1("\n\rPC = '");
    debugPrintStrV1(uintToString(lr_addr,HEXADECIMAL));
    uart_puts("'\n\r");
    debugPrintStrV1("\n\rSP = '");
    debugPrintStrV1(uintToString(sp_addr,HEXADECIMAL));
	uart_puts("'\n\r");

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

    return new_stack;
}
