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

#include "../includes/apolo/commandInterpreter.h"

// Comandos del sistema

static commandInterpreter_t * commandInterpreter;

commandInterpreter_t * getCommandInterpreter(void) {
	return commandInterpreter;
}

static void hello(void) {
	sConsoleWrite(getCurrentSConsole(),"Hola amigo, que tal?");
	return;
}
/*
static char * boot(void) {

}
*/
static void regs(void) {

	uint32_t r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,
			r13,r14,pc,sp;

	// registros a variables.
	asm volatile ("MOV %0, R0\n\t" : "=r" (r1) );
    asm volatile ("MOV %0, R1\n\t" : "=r" (r2) );
	asm volatile ("MOV %0, R2\n\t" : "=r" (r3) );
    asm volatile ("MOV %0, R3\n\t" : "=r" (r4) );
	asm volatile ("MOV %0, R4\n\t" : "=r" (r5) );
    asm volatile ("MOV %0, R5\n\t" : "=r" (r6) );
	asm volatile ("MOV %0, R6\n\t" : "=r" (r7) );
    asm volatile ("MOV %0, R7\n\t" : "=r" (r8) );
	asm volatile ("MOV %0, R8\n\t" : "=r" (r9) );
    asm volatile ("MOV %0, R9\n\t" : "=r" (r10));
	asm volatile ("MOV %0, R10\n\t" : "=r"(r11));
    asm volatile ("MOV %0, R11\n\t" : "=r"(r12));
	asm volatile ("MOV %0, R12\n\t" : "=r"(r13));
    asm volatile ("MOV %0, R13\n\t" : "=r"(r14));
	asm volatile ("MOV %0, PC\n\t" : "=r" (pc) );
    asm volatile ("MOV %0, SP\n\t" : "=r" (sp) );

}

static void reg(unsigned char * r) {

	uint32_t value = 0;

	if(strncmp(r,"r0",3))
		asm volatile ("MOV %0, R0\n\t" : "=r" (value) );
	else if(strncmp(r,"r1",3))
		asm volatile ("MOV %0, R1\n\t" : "=r" (value) );
	else if(strncmp(r,"r2",3))
		asm volatile ("MOV %0, R2\n\t" : "=r" (value) );
	else if(strncmp(r,"r3",3))
		asm volatile ("MOV %0, R3\n\t" : "=r" (value) );
	else if(strncmp(r,"r4",3))
		asm volatile ("MOV %0, R4\n\t" : "=r" (value) );
	else if(strncmp(r,"r5",3))
		asm volatile ("MOV %0, R5\n\t" : "=r" (value) );
	else if(strncmp(r,"r6",3))
		asm volatile ("MOV %0, R6\n\t" : "=r" (value) );
	else if(strncmp(r,"r7",3))
		asm volatile ("MOV %0, R7\n\t" : "=r" (value) );
	else if(strncmp(r,"r8",3))
		asm volatile ("MOV %0, R8\n\t" : "=r" (value) );
	else if(strncmp(r,"r9",3))
		asm volatile ("MOV %0, R9\n\t" : "=r" (value));
	else if(strncmp(r,"r10",3))
		asm volatile ("MOV %0, R10\n\t" : "=r"(value));
	else if(strncmp(r,"r11",3))
		asm volatile ("MOV %0, R11\n\t" : "=r"(value));
	else if(strncmp(r,"r12",3))
		asm volatile ("MOV %0, R12\n\t" : "=r"(value));
	else if(strncmp(r,"r13",3))
		asm volatile ("MOV %0, R13\n\t" : "=r"(value));
	else if(strncmp(r,"pc",3))
		asm volatile ("MOV %0, PC\n\t" : "=r" (value) );
	else if(strncmp(r,"sp",3))
		asm volatile ("MOV %0, SP\n\t" : "=r" (value) );

	strcpy(commandInterpreter->lastCommandOutPutBuffer,r);

	strcpy(commandInterpreter->lastCommandOutPutBuffer+3,"= 0x");

	uintToStringStr(value,HEXADECIMAL,commandInterpreter->lastCommandOutPutBuffer+7);

}

void printPid(void) {
	uart_puts("Pid de la consola = ");
    uart_puts(uintToString(getCurrentProcessPid(),DECIMAL));
	uart_puts("\n\r");
}
/*
static char *  mdump(void * addr, uint32_t size) {

}

static char *  run(void * addr) {

}
*/

/*
 * Se inicializan los comandos del sistema:
 * boot, regs, mdump y run
 */
void init_commandInterpreter(void) {

	// add hello
	strcpy(commandInterpreter->commands[0].name,"hello");
	strcpy(commandInterpreter->commands[0].descrp,"hello");
	strcpy(commandInterpreter->commands[0].usage,"hello");
	commandInterpreter->commands[0].function = (void *) &hello;

	//add regs
	strcpy(commandInterpreter->commands[1].name,"regs");
	strcpy(commandInterpreter->commands[1].descrp,"muestra el valor de todos los registros");
	strcpy(commandInterpreter->commands[1].usage,"regs");
	commandInterpreter->commands[1].function = (void *) &reg;

	// Add Print PID
	strcpy(commandInterpreter->commands[2].name,"cpid");
	strcpy(commandInterpreter->commands[2].descrp,"muestra el pid de la consola");
	strcpy(commandInterpreter->commands[2].usage,"cpid");
	commandInterpreter->commands[2].function = (void *) &printPid;


	commandInterpreter->nCommands = 3;
}


static command_t * searchCommand(const char * name) {
	char i;
	int debug = 0;
	for(i = 0; i < commandInterpreter->nCommands
	&& strncmp(name,commandInterpreter->commands[i].name,MAX_SIZE_COMMAND); i++);

	return (i >= commandInterpreter->nCommands ? NULL : &commandInterpreter->commands[i]);

}

/*
 * ejecuta el comando
 */
void executeCommand(char * name, ...) {

	uart_puts(name);
	command_t * command = searchCommand(name);

	if(command == NULL) {
		sConsoleWrite(getCurrentSConsole(),"No existe el comando!!!!");
	}
	else {
		/* limpiamos buffer de salida */
		unsigned int i = MAX_COMMAND_OUTPUT;
		while(i--) {
			commandInterpreter->lastCommandOutPutBuffer[i] = '\0';
		}
		/* ejecución del comando */
		sConsoleWrite(getCurrentSConsole(),"Hola amigo, que tal?");
	}

	return;
}
