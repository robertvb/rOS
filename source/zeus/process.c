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

#include "../includes/zeus/process.h"

pid_t init_process(proc_t * process, const char *name, uint32_t pc, pid_t ppid, char **args, uint32_t sp, uint32_t stack_index) {

    static pid_t nextPid = 0;

	process->name = name;
	process->pid = nextPid;
	nextPid++;
	process->ppid = ppid;
	process->pc = pc;
	process->sp = sp;
	process->stack_index = stack_index;
	process->rounds = 0;
	process->waited = 0;
	int argc;
	for (argc = 0; args[argc]; argc++);
	process->r0 = (uint32_t)argc;
	process->r1 = (uint32_t)args;

	return 0;

}
