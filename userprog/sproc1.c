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

// Just some counting for easy debug on the screen. Simulate user process.
void main() {

	   register unsigned int pid = 0;
	   char caracter = 0;
	   int to = 5;
	   volatile char * resultBuff = {"           "};

	    volatile char * string = "[PROC1] Hola soy el Proceso 1!";
	    unsigned int addr = (unsigned int) string;
		asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
	    asm volatile("SWI #6");

	    string = "[PROC1] Mi PID es.....";
	    addr = (unsigned int) string;
		asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
	    asm volatile("SWI #6");

	    // llamada a getPID
	    asm volatile("SWI #3");

	    asm volatile("MOV %[pid], R0" : : [pid] "r" (pid) );
	    uintToStringStr(pid,DECIMAL,resultBuff);
	    addr = (unsigned int) resultBuff;
	    asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
	    asm volatile("SWI #6");

	    char var;
	    for (var = 0; var < 10; ++var) {
	    	resultBuff[var] = ' ';
		}
	    resultBuff[var] = '\0';

	    string = "[PROC1] me voy a dormir! \n\r\n\r\0";
	    addr = (unsigned int) string;
		asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
	    asm volatile("SWI #6");

	    unsigned int sleep = 5;
		asm volatile("MOV R0, %[sleep]" : : [sleep] "r" (sleep) );
	    asm volatile("SWI #2");

	    string = "[PROC1] me desperte! Me vuelvo a dormir! \n\r\n\r\0";
	    addr = (unsigned int) string;
		asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
	    asm volatile("SWI #6");

		asm volatile("MOV R0, %[sleep]" : : [sleep] "r" (sleep) );
	    asm volatile("SWI #2");
/*
	    string = "[PROC1] me desperte! Quiero una tecla! \n\r\n\r\0";
	    addr = (unsigned int) string;
		asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
	    asm volatile("SWI #1");

	    asm volatile("SWI #5");

	    string = "[PROC1] rGracias! Quiero otra! \n\r\n\r\0";
	    addr = (unsigned int) string;
		asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
	    asm volatile("SWI #1");

	    asm volatile("SWI #5");
*/
	    string = "[PROC1] Ale, ya he terminado! Adios!";
	    addr = (unsigned int) string;
		asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
	    asm volatile("SWI #6");

	    // llamada al sistema para terminar ejecucion
	    asm volatile("SWI #0");
}
