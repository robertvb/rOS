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

#include "../includes/atenea/mem.h"

/* TODO DEBUG */

#include "../includes/hades/rpi-uart.h"

/* Virtual memory layout
 *
 * 0x00000000 - 0x000fffff (  1 MB) = Sistema operativo (code + data + heap + stack)
 * 0x00100000 - 0x001fffff (511 MB) = User process space
 * 0x02000000 - Perifericos
 *
 * La memoria a partír de 0x00000000 hasta 0x000fffff y de 0x02000000 en adelante
 * no será accesible para los procesos de usuario
 */

/* Necesitamos acceder a la tabla de paginas como memoria fisica
 * mediante puntero (*pagetable).
 *
 * Es importante utilizar la direccion 0x4000 debido a que
 * los 14 bits menos significativos de la tabla de paginas
 * deben de ser cero (exigencia de la arquitectura).
 * Básicamente necesitamos tener la tabla de paginas alineada
 * en un boundary de 16KBytes. Esto NO es una coincidencia:
 * 0x4000 (16KB) + 0x4000 (16KB) = 0x8000 (punto de entrada
 * del kernel). Por tanto, se deduce que el lugar entendido
 * para colocar la tabla de paginas es este.
 *
 */
static unsigned int *initpagetable = (unsigned int * const)0x4000; /* 16K */

static unsigned int *kerneldatatable = (unsigned int * const)0x3c00; /* 1K */

/* Memory locations.
 * Se definen en el script del linker y se establecen en el linkado
 * */
extern unsigned int _physdatastart, _physbssstart, _physbssend;


/* Rutina de inicialización de la tabla de páginas
 * y arranque de la mmu.
 */
void init_vmem(void) {

	register unsigned int x;
	register unsigned int pt_addr;
	register unsigned int control;
	register unsigned int *bss;

	/* La MMU cuenta con dos tablas de paginas. La Tabla 0 empieza en
	 * la direccion 0x00000000, y trabaja con los primeros
	 * 32MB a 4GB del espacio virtual.
	 * La tabla 1 cubre el resto de la memoria.
	 *
	 * Por ahora, usaremos solo la tabla 0 que, por defecto, maneja el
	 * espacio virtual en su totalidad. Despues, el registro especial
	 * de la tabla 0 va a apuntar a la tabla de traduccion especifica
	 * de cada proceso de usuario.
	 */

	/* Set up of the translation table
	 * ARM1176JZF-S manual, 6-39
	 *
	 * La memoria en la RPI esta dividida en 4096 secciones de 1MB c/u.
	 * (Es decir, contamos con marcos de pagina de 12 bits)
	 *
	 * Los privilegios se gestionan mediante los bits APX/AP (3 bits en total)
	 * APX esta en el bit 15 de la difinicion del sector, mientras que AP
	 * estan en los bits 10 and 11.
	 *
	 * APX AP            Privileged    Unprivileged
	 *  1  11 (0x8c00) = read-only     read-only
	 *  1  01 (0x8400) = read-only     no access
	 *  0  10 (0x0800) = read-write    read-only
	 *  0  01 (0x0400) = read-write    no-access
	 *  0  11 (0x0c00) = read-write    read-write (full access)
	 *
	 * eXecute Never (XN) esta en el bit 4 (0x10) - las secciones con este
	 * flag no pueden ser ejecutadas ni por modos privilegiados. En este sentido,
	 * el kernel es capaz de protegerse de si mismo.
	 *
	 * Los bits 0 y 1 identifican en tipo de entrada en la tabla:
	 * 0 = translation fault
	 * 1 = course page table (paginas de 1KB, 4KB o 16KB (big pages)
	 * 2 = section or supersection
	 */

	/* Mapeamos la primera seccion de 1MB para el kernel */

	initpagetable[x] = x<<20 | 0x0400 | 2;

	/* Mapeamos las direcciones 0x00100000 - 0x001fffff vis a vis en
	 * memoria virtual. Este será el espacio para los procesos de usuario.
	 * Sin embargo, en este momento usaremos paginas de 4KB en vez de secciones
	 * de 1MB para evitar fragmentacion interna en el espacio del proceso
	 *
	 * Read/write for privileged mode only
	 */

	for(x=1; x < (0x21000000>>20) ; x++)
	{
		/* Desplazamiento de 20 para obtener el marco. */
		if(x >= (0x20000000>>20) )
		{
			/* Mapeamos la direccion fisica a virtual
			 * Read/write for priviledged modes, no execute
			 */
			initpagetable[x] = x<<20 | 0x0400 | 2;
		}
		else
		{
			/* paginamos vis a vis con full access */
			initpagetable[x] = x<<20 | 0x0c00 | 2;
		}
	}

	for(x= (0x21000000>>20); x < 4096 ; x++)
	{
		/* a partir de aqui no paginamos */
		initpagetable[x] = 0;
	}

	/* Inicializacion del .bss
	 * El bss se encuentra cargado en memoria fisica, pero su contenido
	 * puede estar sucio. Debe ser puesto a cero antes de usarse.
	 */
	bss = &_physbssstart;
	while(bss<&_physbssend)
	{
		*bss = 0;
		bss++;
	}

	pt_addr = (unsigned int) initpagetable;


	/* Translation table 0 - ARM1176JZF-S manual, 3-57 */
	asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));

	/* Translation table 1 */
	asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));

	/* Usar translation table 0 por defecto, por ahora */
	asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));


	/* Set Domain 0 ACL to "Client", enforcing memory permissions
	 * See ARM1176JZF-S manual, 3-64
	 * Every mapped section/page is in domain 0
	 */
	asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x1));

	/* Obtenemos el registro de control */
	asm volatile("mrc p15, 0, %[control], c1, c0, 0" : [control] "=r" (control));
	/* Arrancamos la MMU */
	control |= 1;
	/* Enable ARMv6 MMU features (disable sub-page AP) */
	control |= (1<<23);

	/* Escribimos el valor del registro de nuevo */
	asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));

	/* retorno */
	return;


}
