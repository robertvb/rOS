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

static frameList_t frameList;

void init_pmem() {
	/* inicializamos la lista de huecos */
	int i;
	for(i = 0; i < MAX_FRAMES; i++) {
		frameList.totalFrames[i] = i+1;
	}
	frameList.emptyList = 0;
}

dir_t instance_process(pid_t solicitante,unsigned int size) {
	unsigned int pframes = (size / FRAME_SIZE);
	/* de momento con marcos de  4 KB TODO */

	/* Se crea su tabla de paginas */

	dir_t pagetable = get16kframe(solicitante);

	pagetable[0] = 0<<20 | 0x8c00 | 2;

	while(pframes > 0) {

	}
}

dir_t get4kframe(pid_t solicitante) {
	frame_t nextEmptyFrame = frameList.emptyList;
	frameList.emptyList = frameList.totalFrames[nextEmptyFrame];
	frameList.totalFrames[nextEmptyFrame] = solicitante;
	return (dir_t) frame2dir(nextEmptyFrame);
}

// TODO
dir_t get16KBlock(pid_t solicitante) {
	return 0;
}

// TODO
dir_t get1MBlock(pid_t solicitante) {
	return 0;
}

// TODO
dir_t get16MBlock(pid_t solicitante) {
	return 0;
}

