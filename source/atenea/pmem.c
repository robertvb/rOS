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
	Dir_t sld = (Dir_t) get4kframe(solicitant);
	uart_puts("Obtenido 4k marco, dir: ");
	uart_puts(uintToString((unsigned int) sld,HEXADECIMAL));
	uart_puts("\r\n");

	unsigned int cont;
	for(cont = 0; cont < 256; cont++) {
		sld[cont] = NULL;
	}

	Dir_t lastUsedFrameDir = sld;
	Dir_t gotFramesListPointer = NULL;

	uart_puts("marcos necesarios:");
	uart_puts(uintToString(numFrames,DECIMAL));
	uart_puts("\r\n");

	while(numFrames-- > 0) {

		gotFramesListPointer = (Dir_t) get4kframe(solicitant);
		uart_puts("Obtenido 4k marco, dir: ");
		uart_puts(uintToString((unsigned int) gotFramesListPointer,HEXADECIMAL));
		uart_puts("\r\n");
		if(gotFramesListPointer == NULL) {
			/* nos hemos quedado sin memoria!
			 * por tanto, vamos a devolver la que hemos asignado.
			 */
			while(lastUsedFrameDir != NULL) {
				free4kframe(dir2procFrame((unsigned int) lastUsedFrameDir));
				lastUsedFrameDir = (Dir_t) *lastUsedFrameDir;
			}
			return NULL;
		}
		else {
			*gotFramesListPointer = (unsigned int) lastUsedFrameDir;
		}
	}

	returnValue = gotFramesListPointer;
	/*
	 * Conseguido suficientes marcos para el proceso
	 * Vamos a construir ahora su tabla de paginas.
	 */

	Dir_t fldPagetable = (Dir_t) getNextFld(solicitant);

	unsigned int x = 0;

	fldPagetable[x] = x<<20 | 0x0400 | 2;

	for(x=1; x < 4096 ; x++) {
		fldPagetable[x] = 0; // fault page all the space
	}

	/*
	 * Agrupamos marcos por entradas de primer nivel (1MB)
	 * y construimos la tabla de paginas del proceso
	 */

	unsigned int currentCourseTablePagle;
	unsigned int vAddress4kCount = PROC_BASE_ADDRESS;
	Dir_t currentFramePointer;
	Dir_t lastFramePointer;
	Dir_t firstChangedFrame;
	(unsigned int) sld;
	firstChangedFrame = gotFramesListPointer;
	do {
		currentCourseTablePagle = (unsigned int) gotFramesListPointer >> 12;
		gotFramesListPointer = currentFramePointer = lastFramePointer = firstChangedFrame;

		while(currentFramePointer != sld) {
			if(currentCourseTablePagle == ((unsigned int) currentFramePointer) >> 12) {
				uart_puts("got framelist pointer = ");
				uart_puts(uintToString((unsigned int) gotFramesListPointer,HEXADECIMAL));
				uart_puts("\r\n");
				uart_puts("asginando marco virtual 0x");
				uart_puts(uintToString((currentCourseTablePagle & 0x00000003),HEXADECIMAL));
				uart_puts("\r\n");
				uart_puts("currentFramePointer 0x");
				uart_puts(uintToString((unsigned int) (currentFramePointer),HEXADECIMAL));
				uart_puts("\r\n");
				sld[currentCourseTablePagle & 0x00000003] = (unsigned int) currentFramePointer | 0x0030 | 2; // TODO test full access
				vAddress4kCount+=0x1000;
				lastFramePointer = currentFramePointer;
				currentFramePointer = (Dir_t) *currentFramePointer;
			}
			else if (firstChangedFrame == gotFramesListPointer){
				firstChangedFrame = currentFramePointer;
			}
		}

		uart_puts("dir SLD 0x");
		uart_puts(uintToString((unsigned int) sld,HEXADECIMAL));
		uart_puts("\r\n");

		fldPagetable[(unsigned int) gotFramesListPointer >> 20] = (unsigned int) sld | 1; // TODO test course table page in sld

		if(firstChangedFrame != gotFramesListPointer) {
			sld = (Dir_t) get4kframe(solicitant);
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

	uart_puts(" sp del proceso =");
	uart_puts(uintToString(((unsigned int) gotFramesListPointer) + 0x0ffc,DECIMAL));
	uart_puts("\r\n");

	kInternalExecute("proceso",(Dir_t)((unsigned int) returnValue + 0x54),(unsigned int) gotFramesListPointer + 0x0ffc);

	return returnValue;
}

void free4kframe(Frame_t frame) {
	pframeManager.totalFrames[frame] = pframeManager.emptyList;
	pframeManager.emptyList = frame;
}

unsigned int get4kframe(Pid_t solicitant) {

	Frame_t nextEmptyFrame = pframeManager.emptyList;
	if(nextEmptyFrame == MAX_PROC_FRAMES) {
		return NULL;
	}

	pframeManager.emptyList = pframeManager.totalFrames[nextEmptyFrame];
	pframeManager.totalFrames[nextEmptyFrame] = solicitant;

	// TODO DEBUG
	uart_puts("Dando 4kFrame: ");
	uart_puts(uintToString(nextEmptyFrame,HEXADECIMAL));
	uart_puts("\r\n");

	return procFrame2dir(nextEmptyFrame);
}

unsigned int getNextFld(Pid_t solicitant) {

	Fld_t nextFreeFld = fldManager.emptyList;
	if(nextFreeFld == MAX_PROCS) {
		return NULL;
	}

	fldManager.emptyList = fldManager.totalflDescs[nextFreeFld];
	fldManager.totalflDescs[nextFreeFld] = solicitant;

	return fld2dir(nextFreeFld);
}

// TODO
unsigned int get16KBlock(Pid_t solicitant) {
	return NULL;
}

// TODO
unsigned int get1MBlock(Pid_t solicitant) {
	return NULL;
}

// TODO
unsigned int get16MBlock(Pid_t solicitant) {
	return NULL;
}

// TODO
void mapMem(void) {
	return;
}
