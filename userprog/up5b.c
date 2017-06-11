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

void main() {

    char caracter = 0;
    int to = 5;

    volatile char * string = "\n\r\n\rme voy a dormir! \n\r\n\r\0";
    unsigned int addr = (unsigned int) string;
	asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
    asm volatile("SWI #1");
    
    unsigned int sleep = 5;
	asm volatile("MOV R0, %[sleep]" : : [sleep] "r" (sleep) );
    asm volatile("SWI #2");

    string = "\n\r\n\rme desperte! Me vuelvo a dormir! \n\r\n\r\0";
    addr = (unsigned int) string;
	asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
    asm volatile("SWI #1");

	asm volatile("MOV R0, %[sleep]" : : [sleep] "r" (sleep) );
    asm volatile("SWI #2");

    string = "\n\r\n\rme desperte! Quiero una tecla! \n\r\n\r\0";
    addr = (unsigned int) string;
	asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
    asm volatile("SWI #1");

    asm volatile("SWI #5");

    string = "\n\r\n\rGracias! Quiero otra! \n\r\n\r\0";
    addr = (unsigned int) string;
	asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
    asm volatile("SWI #1");

    asm volatile("SWI #5");

    string = "\n\r\n\rme he terminado! \0\n\r\n\r";
    addr = (unsigned int) string;
	asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );
    asm volatile("SWI #1");

    // llamada al sistema para terminar ejecucion
    asm volatile("SWI #0");

    // Call software interrupt #0 (terminate)
}
