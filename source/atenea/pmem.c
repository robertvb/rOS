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

#include "../includes/atenea/pmem.h"

static ProcFrameManager_t pframeManager;
static FlDescManager_t fldManager;

void init_pmem(void) {

	int i;
	/* inicializamos la lista de huecos */
	for (i = 0; i < MAX_PROCS; ++i) {
		fldManager.totalflDescs[i] = i+1;
	}

	/* inicializamos la lista de huecos */
	for (i = 0; i < MAX_PROC_FRAMES; ++i) {
		pframeManager.totalFrames[i] = i+1;
	}

	fldManager.emptyList = pframeManager.emptyList = 0;

}

Dir_t instance_process(Pid_t solicitante, unsigned int size) {

	/* si se ha alcanzado el numero maximo de descriptores o no quedan marcos de uso general
	 * se retorna NULL.
	 */
	if(fldManager.emptyList >= MAX_PROCS || pframeManager.emptyList >= MAX_PROC_FRAMES) {
		return 0x0;
	}

	Fld_t nextFldEmptyAux = fldManager.totalflDescs[fldManager.emptyList];

	unsigned int * fld = (unsigned int *) fld2dir(nextFldEmptyAux);

	/* se mapea el espacio del kernel */

	fld[0] = 0<<20 | 0x0400 | 2;

}

Dir_t get4kframe(Pid_t solicitante) {
	//frame_t nextEmptyFrame = frameManager.emptyList;
	//frameManager.emptyList = frameManager.totalFrames[nextEmptyFrame];
	//frameManager.totalFrames[nextEmptyFrame] = solicitante;
	//return (dir_t) frame2dir(nextEmptyFrame);
}

// TODO
Dir_t get16KBlock(Pid_t solicitante) {
	return 0;
}

// TODO
Dir_t get1MBlock(Pid_t solicitante) {
	return 0;
}

// TODO
Dir_t get16MBlock(Pid_t solicitante) {
	return 0;
}
