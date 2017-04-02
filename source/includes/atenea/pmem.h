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
#include "mem-utils.h"

/*
 * Se contemplan 125 K marcos de 4 KB para procesos de usuario:
 * Espacio total de procesos de usuario igual a 500 MB.
 * En 1 MB se hubican 256 marcos de 4 KB. Por tanto:
 * MAX_FRAMES = 125 K = 128000 = 256 * 500.
 */

#define MAX_PROC_FRAMES   0x0001f400 /* 125 KMarcos */
#define PROC_FRAME_SIZE   0x00001000 /* 4 KB */
#define PROC_BASE_ADDRESS 0x00100000 /* @1 MB */

#define procFrame2dir(frame) PROC_BASE_ADDRESS + (PROC_FRAME_SIZE * frame) /* 1 MB  + (4 KB * Marco) */
#define dir2procFrame(dir) (dir - PROC_BASE_ADDRESS) / PROC_FRAME_SIZE /* (dir - 1 MB) / 4kb   */

/*
 * Se contemplan 704 procesos de usuario como maximo:
 * Espacio total de Descriptores de primer nivel (FLD) igual a 11 MB.
 * En 1 MB se hubican 64 bloques de 16 KB. Por tanto:
 * MAX_PROC = 125 K = 128000 = 256 * 500.
 */
#define MAX_PROCS                 704 /* Maximo numero de procesos determinado por maximo numero de FLD */
#define FLD_SIZE           0x00004000 /* 16 KB */
#define FLD_BASE_ADDRESS   0x1f500000 /* @501 MB */

#define fld2dir(flDesc) FLD_BASE_ADDRESS + (FLD_SIZE * flDesc) /* @501 MB  + (16 KB * descriptor) */

typedef unsigned int * Dir_t;
typedef int Frame_t;
typedef short Fld_t;

typedef struct {
	Frame_t totalFrames[MAX_PROC_FRAMES];
	Frame_t emptyList;
} ProcFrameManager_t;

typedef struct {
	Fld_t totalflDescs[MAX_PROCS];
	Fld_t emptyList;
} FlDescManager_t;

void init_pmem(void);

/*
 * Los procesos pueden reservar memoria en bloques de memoria mayores a 4 KB:
 * Con objeto de aprovechar las capacidades de la arquitectura, y utilizando
 * la posibilidad de mapear memoria en bloques mayores a los 4 KB, los procesos
 * son capaces de solicitar hasta 16 MB por bloque. Sin embargo,
 * a ojos del sistema operativo, el proceso estara solicitando un total de
 * 256 bloques de 4 KB + los necesarios para su mapeo (y aqui esta el ahorro)
 */

Dir_t instance_process(Pid_t solicitante, unsigned int size);

void free4kframe(Frame_t frame);

Dir_t getNextFld(Pid_t solicitant);

Dir_t get4kframe(Pid_t solicitante);

Dir_t get16KBlock(Pid_t solicitante);

Dir_t get1MBlock(Pid_t solicitante);

Dir_t get16MBlock(Pid_t solicitante);

void mapMem(void);

#endif /* SOURCE_INCLUDES_ATENEA_PMEM_H_ */
