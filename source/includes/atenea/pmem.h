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

#ifndef PMEM_H
#define PMEM_H

#include "../zeus/process.h"

/*
 * Se contemplan 125 K marcos de 4 kB para procesos de usuario:
 * Espacio total de procesos de usuario igual a 500 MB.
 * En 1 MB se hubican 256 marcos de 4 KB. Por tanto:
 * MAX_FRAMES = 125 K = 128000 = 256 * 500.
 */

#define MAX_PS_FRAMES      0x0001f400 /* 125 KMarcos */
#define PS_FRAME_SIZE      0x00001000 /* 4 KB */
#define PS_BASE_ADDRESS    0x00100000 /* 1 MB */

/* First Level Descriptors Address */
#define FL_DSCRPTS_ADDRESS 0x1f500000 /* 501 MB */

#define frame2dir(frame) BASE_ADDRESS + (FRAME_SIZE * frame) /* 1 MB  + (4 KB * Marco) */

typedef unsigned int * dir_t;
typedef unsigned int frame_t;

typedef struct {
	frame_t totalFrames[MAX_PS_FRAMES];
	dir_t emptyList;
} frameManager_t;

/*
 * Los procesos pueden reservar memoria en bloques de memoria mayores a 4 KB:
 * Con objeto de aprovechar las capacidades de la arquitectura, y utilizando
 * la posibilidad de mapear memoria en bloques mayores a los 4 KB, los procesos
 * son capaces de solicitar hasta 16 MB por bloque. Sin embargo,
 * a ojos del sistema operativo, el proceso estara solicitando un total de
 * 256 bloques de 4 KB + los necesarios para su mapeo (y aqui esta el ahorro)
 */

void init_pmem();

dir_t instance_process(pid_t solicitante, unsigned int size);

dir_t get4kframe(pid_t solicitante);

dir_t get16KBlock(pid_t solicitante);

dir_t get1MBlock(pid_t solicitante);

dir_t get16MBlock(pid_t solicitante);

#endif /* SOURCE_INCLUDES_ATENEA_PMEM_H_ */
