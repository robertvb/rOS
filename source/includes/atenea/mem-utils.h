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

#ifndef MEM_UTILS_H
#define MEM_UTILS_H

#include <stdint.h>

/* Virtual memory layout
 *
 * 0x00000000 - 0x7fffffff (0-2GB) = memoria de procesos de usuario
 * 0x80000000 - 0xa0ffffff (2GB) = memoria fisica
 *  incluyendo los periféricos situados en 0x20000000 - 0x20ffffff
 * 0xc0000000 - 0xefffffff = kernel heap/stack
 * 0xf0000000 - 0xffffffff = kernel code

 * La memoria a partír de 0x80000000
 * no será accesible para los procesos de usuario
 */

#define NULL    0x00000000

/* Rutina para convertir una dirección virtual en fisica
 *  siguiendo la tabla de páginas.

 * Retorna la dirección fisica o 0xffffffff si dicha dir. virtual
 * no se encuentra mapeada.
 */
uint32_t mem_v2p(unsigned int);

#define mem_p2v(X) (X+0x80000000)

#endif /* SOURCE_INCLUDES_ATENEA_MEM_UTILS_H_ */
