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

#ifndef PROCESS_H
# define PROCESS_H


# include <stdint.h>
# include "../atenea/mem.h"

# define STACK_SIZE 2048 // 2KB

typedef int32_t pid_t;

/*
 * Proceso del sistema
 * Es importante mantener un boundary de 32bit,
 * es decir, de una palabra para favoerecer el
 * performance en los accesos.
 */
typedef struct proc_t
{
	const char * name;
    pid_t pid;
    pid_t ppid;
    uint32_t pc;
    uint32_t sp;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t stack_index;
    //uint32_t heap_index;
    uint32_t waited;
    uint32_t rounds;
} proc_t;

pid_t init_process(proc_t * process, const char *name, uint32_t pc, pid_t ppid, char **args, uint32_t sp, uint32_t stack_index);


void save_registers(proc_t * process);

void execute(proc_t * process);

//TODO:
int fork_exec(const char *path, char **args); //returns pid

#endif /* !PROCESS_H */
