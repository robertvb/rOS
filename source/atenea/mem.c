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
 * 0x00000000 - 0x7fffffff (0-2GB) = memoria de procesos de usuario
 * 0x80000000 - 0xa0ffffff (2GB) = memoria fisica
 *  incluyendo los periféricos situados en 0x20000000 - 0x20ffffff
 * 0xc0000000 - 0xefffffff = kernel heap/stack
 * 0xf0000000 - 0xffffffff = kernel code
 *
 * La memoria a partír de 0x80000000
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
__attribute__((naked)) void init_vmem(void) {

	asm volatile("push {r0, r1, r2}");

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
	 * Muchas de ellas se encuentran unmaped, es decir, son prefetch o
	 * data aborts, excepto 0x8000000-0xa1000000, que esta
	 * mapeada en 0x00000000-0x2a000000 (memoria fisica y perifericos),
	 * y el codigo del kernel.
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
	 *
	 * eXecute Never (XN) esta en el bit 4 (0x10) - las secciones con este
	 * flag no pueden ser ejecutadas ni por modos privilegiados. En este sentido,
	 * el kernel es capaz de protegerse de si mismo.
	 *
	 * Los bits 0 y 1 identifican en tipo de entrada en la tabla:
	 * 0 = translation fault
	 * 1 = course page table
	 * 2 = section or supersection
	 */

	for(x=0; x<4096; x++)
	{
		/* Desplazamiento de 20 para obtener el marco. */
		if((x >= (0x80000000>>20)) && (x < (0xa1000000>>20)))
		{
			/* Mapeamos la direccion fisica a virtual
			 * Read/write for priviledged modes, no execute
			 */
			initpagetable[x] = (x-2048)<<20 | 0x0410 | 2;
		}
		else
		{
			/* No entry in table; translation fault */
			initpagetable[x] = 0;
		}
	}

	uart_puts("traza 1\n");

	/* Mapeamos las direcciones 0x00000000-0x000fffff vis a vis en
	 * memoria virtual. Esto es temporal: aqui es donde el codigo esta
	 * ejecutandose ahora. Cuando esta rutina termine y el kernel se encuentre
	 * en 0xf0000000 ya cambiaremos este mapeo.
	 *
	 * Read/write for privileged mode only
	 */

	initpagetable[0] = 0<<20 | 0x0400 | 2;

	/* Mapeamos ahora 1MB para el codigo del kernel (0xf00000000-0xf00fffff).
	 * En el script del linker se define que se cargara en 0x8000 y
	 * estara disponible en memoria virtual a partir de la direccion
	 * 0xf0008000. Como hemos limitado el codigo del kernel a 1MB con una unica
	 * entrada en la tabla podemos abarcarlo en su totalidad.
	 * (por lo menos mientras el codigo del kernel no crezca mas 1MB),
	 * De esta manera simplificamos y reducimos el numero de entradas en la TLB.
	 *
	 * Map as read-only for privileged modes only
	 */
	initpagetable[3840] = 0<<20 | 0x8400 | 2;

	/* Las direcciones 0xc0000000-0xc00fffff se mapean con una course page table
	 *
	 * Esto mapea el seccion data del kernel de donde ha sido cargada en memoria
	 * (justo despues del kernel code, por ej. 0x0001f000) a 0xc0000000 (virtual)
	 * Solo la memoria en uso se mapea (a los siguientes 4K). El resto de la tabla
	 * no se mapea.
	 */
	initpagetable[3072] = 1 | (uint32_t)kerneldatatable;

	/* Populate kerneldatatable - see ARM1176JZF-S manual, 6-40
	 *
	 * LOs bit APX/AP para una entrada estan en los bits bits 9 y 4&5.
	 * El significado es el mismo que para una entrada de seccion (1MB).
	 *
	 * 0 01 (0x0010) =  read-write privileded modes, no access otherwise
	 *
	 * Los bits 0 y 1 determinan el tipo de pagina:
	 * 0 = unmapped, translation fault
	 * 1 = large page (64K)			(XN is bit 15)
	 * 2 = small page (4K), executable	(XN is bit 0)
	 * 3 = small page (4K), not-executable  (XN is bit 0)
	 *
	 * 256 entries, one for each 4KB in the 1MB covered by the table
	 */
	for(x=0; x<256; x++)
	{
		/* &_physbssend is the physical address of the end of the
		 * kernel data - somewhere between 0x00009000 and 1MB (any
		 * more than that and this code will need rewriting...)
		 */
		if(x <= ((unsigned int)&_physbssend >> 12))
			kerneldatatable[x] = ((unsigned int)&_physdatastart + (x<<12)) | 0x0010 | 2;
		else
			kerneldatatable[x] = 0;
	}

	uart_puts("traza 2\n");
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

	uart_puts("traza 3\n");

	/* Translation table 0 - ARM1176JZF-S manual, 3-57 */
	asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));

	uart_puts("traza 4\n");
	/* Translation table 1 */
	asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));

	uart_puts("traza 5\n");
	/* Usar translation table 0 por defecto, por ahora */
	asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));

	uart_puts("traza 6\n");

	/* Set Domain 0 ACL to "Client", enforcing memory permissions
	 * See ARM1176JZF-S manual, 3-64
	 * Every mapped section/page is in domain 0
	 */
	asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x1));

	uart_puts("traza 7\n");

	/* Obtenemos el registro de control */
	asm volatile("mrc p15, 0, %[control], c1, c0, 0" : [control] "=r" (control));
	/* Arrancamos la MMU */
	control |= 1;
	/* Enable ARMv6 MMU features (disable sub-page AP) */
	control |= (1<<23);

	uart_puts("traza 8\n");
	/* Escribimos el valor del registro de nuevo */
	asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));

	uart_puts("traza 9\n");
	/* retorno */
	asm volatile("pop {r0, r1, r2}");
	asm volatile("bx lr");

}
