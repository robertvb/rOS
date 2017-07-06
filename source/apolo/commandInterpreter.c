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
static unsigned char argv[MAX_SIZE_ARGV][MAX_SIZE_ARG];

commandInterpreter_t * getCommandInterpreter(void) {
	return commandInterpreter;
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

}

void printPid(void) {
	uart_puts("Pid de la consola = ");
    uart_puts(uintToString(getCurrentProcessPid(),DECIMAL));
	uart_puts("\n\r");
}

void echo(unsigned int argc, unsigned char (* argv_pointer)[MAX_SIZE_ARG]) {
	sConsoleWrite(getCurrentSConsole(),argv_pointer[1]);
}

void apariencia(unsigned int argc, unsigned char (* argv_pointer)[MAX_SIZE_ARG]) {

	if(!strncmp("lima",argv_pointer[1],stringLength("lima"))) {
		setSConsolebackGroundColour(getCurrentSConsole(),0xb691);
		setSConsoleFontColour(getCurrentSConsole(),0x0000);
		uart_puts("<<< cambiando a estilo lima>>>>>>>");
	}else if(!strncmp("ros",argv_pointer[1],stringLength("ros"))) {
		setSConsolebackGroundColour(getCurrentSConsole(),0x2844);
		setSConsoleFontColour(getCurrentSConsole(),0xAFE5);
		uart_puts("<<< cambiando a estilo ros>>>>>>>");
	}else if(!strncmp("nyb",argv_pointer[1],stringLength("nyb"))) {
		setSConsolebackGroundColour(getCurrentSConsole(),0x0000);
		setSConsoleFontColour(getCurrentSConsole(),0xFFFF);
		uart_puts("<<< cambiando a estilo nyb>>>>>>>");
	}else if(!strncmp("ayb",argv_pointer[1],stringLength("ayb"))) {
		setSConsolebackGroundColour(getCurrentSConsole(),0x000F);
		setSConsoleFontColour(getCurrentSConsole(),0xFFFF);
		uart_puts("<<< cambiando a estilo ayb>>>>>>>");
	}

	focusSConsole(getCurrentSConsole());
}

void clear(unsigned int argc, unsigned char (* argv_pointer)[MAX_SIZE_ARG]) {
	clearSConsole(getCurrentSConsole());
}

void ls(unsigned int argc, unsigned char (* argv_pointer)[MAX_SIZE_ARG]) {
	sConsoleWrite(getCurrentSConsole(),getCurrentDirStr());
}

void exec(unsigned int argc, unsigned char (* argv_pointer)[MAX_SIZE_ARG]) {
	unsigned int bDir = getFileBuf(argv_pointer[1]);
	if(bDir == NULL) {
		sConsoleWrite(getCurrentSConsole(),"No se encuentra el fichero!");
		return;
	}

	kInternalExecute(argv_pointer[1],bDir);
	return;
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

	unsigned char i = 0;
	for (i = 0; i < MAX_SIZE_ARGV; ++i) {
		argv[i][0] = '\0';
	}

	commandInterpreter->argv_pointer = argv;
	commandInterpreter->commandBuffer[0] = '\0';

	// Add ls
	strcpy(commandInterpreter->commands[0].name,"ls");
	strcpy(commandInterpreter->commands[0].descrp,"muestra elementos del directorio actual");
	strcpy(commandInterpreter->commands[0].usage,"ls");
	commandInterpreter->commands[0].argc = 1;
	commandInterpreter->commands[0].function = (void *) &ls;

	// Add clear
	strcpy(commandInterpreter->commands[1].name,"clear");
	strcpy(commandInterpreter->commands[1].descrp,"limpia el terminal");
	strcpy(commandInterpreter->commands[1].usage,"clear");
	commandInterpreter->commands[1].argc = 1;
	commandInterpreter->commands[1].function = (void *) &clear;

	// Add Print PID
	strcpy(commandInterpreter->commands[2].name,"exec");
	strcpy(commandInterpreter->commands[2].descrp,"ejecuta el proceso seleccionado");
	strcpy(commandInterpreter->commands[2].usage,"exec 'file'.elf");
	commandInterpreter->commands[2].argc = 2;
	commandInterpreter->commands[2].function = (void *) &exec;

	// Add echo
	strcpy(commandInterpreter->commands[3].name,"echo");
	strcpy(commandInterpreter->commands[3].descrp,"repite el mensaje enviado");
	strcpy(commandInterpreter->commands[3].usage,"echo 'str'");
	commandInterpreter->commands[3].argc = 2;
	commandInterpreter->commands[3].function = (void *) &echo;

	// Add apariencia
	strcpy(commandInterpreter->commands[4].name,"estilo");
	strcpy(commandInterpreter->commands[4].descrp,"cambia la apariencia al estilo seleccionado");
	strcpy(commandInterpreter->commands[4].usage,"estilo 'name' donde 'name' = {navajo, ros, nyb, ayb}");
	commandInterpreter->commands[4].argc = 2;
	commandInterpreter->commands[4].function = (void *) &apariencia;

	commandInterpreter->nCommands = 5;
}


static command_t * searchCommand(const char * name) {

	char i;
	for(i = 0; i < commandInterpreter->nCommands
	&& strncmp(name,commandInterpreter->commands[i].name,MAX_SIZE_COMMAND); i++);

	uart_puts("<<< VALOR DE i = ");
	uart_puts(uintToString(i,DECIMAL));
	uart_puts(">>>>>>\n\r");

	uart_puts("<<< VALOR DE commandInterpreter->commands[i].name = '");
	uart_puts(commandInterpreter->commands[i].name);
	uart_puts("'>>>>>>\n\r");

	return (i >= commandInterpreter->nCommands ? NULL : &commandInterpreter->commands[i]);

}

/*
 * ejecuta el comando
 */
void executeCommand(unsigned char * command) {

	strcpy(commandInterpreter->commandBuffer,command);
	unsigned char * commandInfo = commandInterpreter->commandBuffer; /* SPROMPT LEN */


	char i;
	for(i = 0; i < commandInterpreter->nCommands
	&& strncmp(commandInfo,commandInterpreter->commands[i].name,stringLength(commandInterpreter->commands[i].name)); i++);


	if(i == commandInterpreter->nCommands) {
		sConsoleWrite(getCurrentSConsole(),"No existe el comando!!!!");

	}
	else {
		/* creamos la cadena de argumentos (argv) */
		char j,v,last;
		for (j = 0, v = 0, last = 0; commandInfo[j] != '\0' && v < commandInterpreter->commands[i].argc - 1; j++) {
			if(commandInfo[j] == ' ') {
				commandInfo[j] = '\0';
				strcpy(argv[v],&commandInfo[last]);
				last = j + 1;
				v++;
			}
		}

		strcpy(argv[v],&commandInfo[last]);

		commandInterpreter->commands[i].function(commandInterpreter->commands[i].argc, argv);

		for (i = 0; i < commandInterpreter->commands[i].argc; ++i) {
			argv[i][0] = '\0';
		}

		commandInterpreter->commandBuffer[0] = '\0';

	}

	return;
}
