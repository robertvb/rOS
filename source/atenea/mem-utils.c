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

#include "../includes/atenea/mem-utils.h"



static unsigned int *pagetable = (unsigned int * const) 0x4000; /* 16k */

void invalidate_TLB0(void) {
	asm volatile("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r" (0));
}

/* Rutina para convertir una dirección virtual en fisica
 *  siguiendo la tabla de páginas.

 * Retorna la dirección fisica o 0xffffffff si dicha dir. virtual
 * no se encuentra mapeada.
 */
uint32_t mem_v2p(unsigned int virtualaddr) {
	unsigned int pt_data = pagetable[virtualaddr >> 20];
	unsigned int cpt_data, physaddr;

	if((pt_data & 3)==0 || (pt_data & 3)==3)
	{
		/* Nothing mapped */
		return 0xffffffff;
	}

	if((pt_data & 3)==2)
	{
		/* (Super)Section */

		physaddr = pt_data & 0xfff00000;

		if(pt_data & (1<<18))
		{
			/* 16MB Supersection */
			physaddr += virtualaddr & 0x00ffffff;
		}
		else
		{
			/* 1MB Section */
			physaddr += virtualaddr & 0x000fffff;
		}

		return physaddr;
	}

	/* Coarse page table */
	cpt_data = ((unsigned int *)(0x80000000 + (pt_data & 0xfffffc00)))[(virtualaddr>>12)&0xff] ;

	if((cpt_data & 3) == 0)
	{
		/* Nothing mapped */
		return 0xffffffff;
	}

	if(cpt_data & 2)
	{
		/* Small (4k) page */
		return (cpt_data & 0xfffff000) + (virtualaddr & 0xfff);
	}

	/* Large (64k) page */
	return (cpt_data & 0xffff0000) + (virtualaddr & 0xffff);
}

void memncpy(void *dest, void *src, unsigned int n) {
   // Typecast src and dest addresses to (char *)
   char *csrc = (char *)src;
   char *cdest = (char *)dest;

   // Copy contents of src[] to dest[]
   unsigned int i;
   for(i=0; i<n; i++)
       cdest[i] = csrc[i];
}
