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
#include "../atenea/pmem.h"

// todo DEBUG
#include "../hades/rpi-uart.h"


Pid_t getCurrentProcessPid(void);
Pid_t getCurrentProcessPpid(void);
Pid_t getNextPid(void);
Process_t * getProcUsingPid(Pid_t);
void init_proc_scheduling(void);
void kInternalExecute(char * name,  Dir_t addr);
unsigned int schedule_timeout(unsigned int stack_pointer, unsigned int pc, unsigned int spsr);
unsigned int sleepCurrentProc(unsigned int addr,unsigned int sp, unsigned int spsr, unsigned int tics);
unsigned int terminate_process(void);
void halt(void);
unsigned int uart_interrupt_handler(unsigned int stack_pointer, unsigned int pc); // TODO
unsigned int getCharacterHandler(unsigned int pc, unsigned int sp, unsigned int spsr); // TODO

#endif
