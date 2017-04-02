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

/*
 * Retorna la direccion de la primera instruccion del proceso o NULL en caso de memoria insuficiente
 */
Dir_t instance_process(Pid_t solicitant, unsigned int size) {

	/* si se ha alcanzado el numero maximo de descriptores o no quedan marcos de uso general
	 * se retorna NULL.
	 */
	Dir_t returnValue = NULL;

	if(fldManager.emptyList >= MAX_PROCS || pframeManager.emptyList >= MAX_PROC_FRAMES) {
		return NULL;
	}

	uart_puts("instanciando proceso! \n\r");

	unsigned int numFrames = (size / PROC_FRAME_SIZE) + 1;
	Dir_t sld = get4kframe(solicitant);

	unsigned int cont;
	for(cont = 0; cont < 256; cont++) {
		* sld = NULL;
	}

	Dir_t lastUsedFrameDir = sld;
	Dir_t gotFramesListPointer = NULL;

	uart_puts("marcos necesarios:");
	uart_puts(uintToString(numFrames,DECIMAL));
	uart_puts("\r\n");

	while(numFrames-- > 0) {
		gotFramesListPointer = get4kframe(solicitant);
		if(gotFramesListPointer == NULL) {
			/* nos hemos quedado sin memoria!
			 * por tanto, vamos a devolver la que hemos asignado.
			 */
			while(lastUsedFrameDir != NULL) {
				free4kframe(dir2procFrame((unsigned int) lastUsedFrameDir));
				lastUsedFrameDir = (Dir_t) * lastUsedFrameDir;
			}
			return NULL;
		}
		else {
			* gotFramesListPointer = (unsigned int) lastUsedFrameDir;
		}
	}

	returnValue = gotFramesListPointer;
	/*
	 * Conseguido suficientes marcos para el proceso
	 * Vamos a construir ahora su tabla de paginas.
	 */

	Dir_t fldPagetable = getNextFld(solicitant);

	unsigned int x = 0;

	for(x=0; x < 4096 ; x++) {
		fldPagetable[x] = x<<20 | 0x400 | 2;
	}

	/*
	 * Agrupamos marcos por entradas de primer nivel (1MB)
	 * y construimos la tabla de paginas del proceso
	 */

	unsigned int currentCourseTablePagle;
	Dir_t currentFramePointer;
	Dir_t lastFramePointer;
	Dir_t firstChangedFrame;

	firstChangedFrame = gotFramesListPointer;
	do {
		currentCourseTablePagle = (unsigned int) gotFramesListPointer >> 20;
		gotFramesListPointer = currentFramePointer = lastFramePointer = firstChangedFrame;

		while(currentFramePointer != sld) {
			if(currentCourseTablePagle == ((unsigned int) currentFramePointer) >> 20) {
				sld[currentCourseTablePagle << 20] = (unsigned int) gotFramesListPointer; // TODO bits de control
				lastFramePointer = currentFramePointer;
				currentFramePointer = (Dir_t) * currentFramePointer;
			}
			else if (firstChangedFrame == gotFramesListPointer){
				firstChangedFrame = currentFramePointer;
			}
		}

		fldPagetable[currentCourseTablePagle] = (unsigned int) sld; // TODO BITS DE CONTROL

		if(firstChangedFrame != gotFramesListPointer) {
			sld = get4kframe(solicitant);
		}

	} while(firstChangedFrame != gotFramesListPointer);

	uart_puts("estado pframeManager.totalFrames[0]=");
	uart_puts(uintToString(pframeManager.totalFrames[0],DECIMAL));
	uart_puts(" estado pframeManager.emptyList=");
	uart_puts(uintToString(pframeManager.emptyList,DECIMAL));
	uart_puts("\r\n");
	uart_puts("estado fldManager.totalflDescs[0]=");
	uart_puts(uintToString(fldManager.totalflDescs[0],DECIMAL));
	uart_puts(" estado fldManager.emptyList=");
	uart_puts(uintToString(fldManager.emptyList,DECIMAL));
	uart_puts("\r\n");

	return returnValue;
}

void free4kframe(Frame_t frame) {
	pframeManager.totalFrames[frame] = pframeManager.emptyList;
	pframeManager.emptyList = frame;
}

Dir_t get4kframe(Pid_t solicitant) {

	Frame_t nextEmptyFrame = pframeManager.emptyList;
	if(nextEmptyFrame == MAX_PROC_FRAMES) {
		return NULL;
	}

	pframeManager.emptyList = pframeManager.totalFrames[nextEmptyFrame];
	pframeManager.totalFrames[nextEmptyFrame] = solicitant;

	// TODO DEBUG
	uart_puts("Dando 4kFrame: ");
	uart_puts(uintToString(pframeManager.emptyList,DECIMAL));
	uart_puts("\r\n");

	return (Dir_t) procFrame2dir(nextEmptyFrame);
}

Dir_t getNextFld(Pid_t solicitant) {

	Fld_t nextFreeFld = fldManager.emptyList;
	if(nextFreeFld == MAX_PROCS) {
		return NULL;
	}

	fldManager.emptyList = fldManager.totalflDescs[nextFreeFld];
	fldManager.totalflDescs[nextFreeFld] = solicitant;

	return (Dir_t) fld2dir(nextFreeFld);
}

// TODO
Dir_t get16KBlock(Pid_t solicitant) {
	return NULL;
}

// TODO
Dir_t get1MBlock(Pid_t solicitant) {
	return NULL;
}

// TODO
Dir_t get16MBlock(Pid_t solicitant) {
	return NULL;
}

// TODO
void mapMem(void) {
	return;
}
