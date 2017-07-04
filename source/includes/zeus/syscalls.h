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

#ifndef SYSCALLS_H
#define SYSCALLS_H


#include "../atenea/pmem.h"
#include "scheduler.h"
#include "../hades/rpi-uart.h"
#include "../apolo/screenConsole.h"


#define SC_EXIT						0
#define SC_UART_WRITE				1
#define SC_SLEEP					2
#define SC_GET_PID					3
#define SC_GET_PPID					4
#define SC_GET_CHAR					5
#define SC_TERMINAL_WRITE			6

typedef unsigned int (*system_call_t)(unsigned int pc, unsigned int sp, unsigned int spsr);

typedef struct system_call_entry {
	uint32_t swi;
    char* name;
    system_call_t function;
    uint32_t flags;
    uint32_t params;
} system_call_entry_t;


void init_syscalls(void);

unsigned int syscall_handler(unsigned int sp_addr,unsigned int lr_addr,unsigned int spsr, unsigned int swi);

#endif /* SOURCE_INCLUDES_ZEUS_SYSCALLS_H_ */
