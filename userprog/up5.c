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

    volatile char * string = "Mi PID es: \0";
    unsigned int addr = (unsigned int) string;

	asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );

    asm volatile("SWI #1");
    
    string = "Dame un caracter: \0";
    addr = (unsigned int) string;
    do {
    	asm volatile("MOV R0, %[dir]" : : [dir] "r" (addr) );

    	asm volatile("SWI #5");
    
    	asm volatile ("MOV %0, R0 \n\t" : "=r" (caracter) );
    } while(caracter != 'r');

    unsigned int sleep = 5;
	asm volatile("MOV R0, %[sleep]" : : [sleep] "r" (sleep) );
    asm volatile("SWI #2");

    // llamada al sistema para terminar ejecucion
    asm volatile("SWI #0");

    // Call software interrupt #0 (terminate)
}
