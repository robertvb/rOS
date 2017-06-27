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

#include "../includes/hades/rpi-gpio.h"
#include "../includes/hades/rpi-systimer.h"
#include "../includes/poseidon/rpi-armtimer.h"
#include "../includes/poseidon/rpi-interrupts.h"
#include "../includes/zeus/scheduler.h"
#include "../includes/hades/rpi-uart.h"
#include "../includes/apolo/uartConsole.h"
#include "../includes/atenea/kmem.h"
#include "../includes/atenea/pmem.h"
#include "../includes/hades/emmc.h"
#include "../includes/atenea/fat32.h"
#include "../includes/atenea/fs.h"
#include "../includes/atenea/mem-utils.h"

int main(uint32_t r0, uint32_t r1, uint32_t atagsAddr) {

	uart_init();
	uart_puts("Hello, it is rOS :)!\r\n");

#if(0)
	uart_puts("Inicializando driver emmc......\r\n");

	struct block_device *bd = NULL;
	if(emmc_card_init(&bd) == 0) {
		uart_puts("emmc_sd_card_driver inicializado!!!!!!\r\n");
		uart_puts("block Size = ");
		uart_puts(uintToString(bd->block_size,DECIMAL));
		uart_puts("\r\n");
	}
	else {
		uart_puts("[ERROR] emmc_sd_card_driver NO inicializado!!!!!!\r\n");
	}
	uint8_t buffer [512];
	size_t * pointer = (size_t *) buffer;
	unsigned int i;
	for (i = 0; i < 512; ++i) {
		buffer[i] = 0;
	}
	if(emmc_read(bd,buffer,bd->block_size,0) == -1) {
		uart_puts("[ERROR] emmc_bd error en la lectura!!!!!\r\n");
	}
	else {
		uart_puts("Bloque leido con exito!!!!!\r\n");
		uart_puts("Dumpeo hexadecimal del MBR: \r\n");

		for (i = 0; i < 128; ++i) {
			uart_puts(uintToString(i*4,DECIMAL));
			uart_puts(":[");
			uart_puts(uintToString(pointer[i],HEXADECIMAL));
			uart_puts("]\r\n");
		}

		uart_puts("Vamos a leer el PBR:!!!!!\r\n");
		mbr_t * ptrmbr = buffer;

		uart_puts("SIZEOF MBR =");
		uart_puts(uintToString(sizeof(mbr_t),DECIMAL));
		uart_puts("\r\n");
		uart_puts("Primer sector de la particion =");

		uint32_t primerSector = ptrmbr->descParticion[0].primerSector;

		uart_puts(uintToString(primerSector,HEXADECIMAL));
		uart_puts("\r\n");

		if(emmc_read(bd,buffer,bd->block_size,primerSector) == -1) {
			uart_puts("[ERROR] emmc_bd error en la lectura!!!!!\r\n");
		}
		else {
			pbr_t * ptrpbr = buffer;
			uint32_t primerSectorFAT = primerSector + ptrpbr->bpb.sectoresReservados;

			uart_puts("Sectores de la particion =");
			uart_puts(uintToString(ptrpbr->bpb.sectores,DECIMAL));
			uart_puts("\r\n");

			/* un cluster es una agruàcion de sectores contiguos, en este caso 64 */

			/* 15515648 Sectores particion / 64 Sectores/cluster = 242432 clusters particion */

			uint32_t fat[242432];

			for(i = 0; i < 242432 / (512/4); i++) {
				emmc_read(bd,&fat[i*(512/4)],512,primerSectorFAT+i);
			}

			/* ya tenemos la FAT ahora vamos a buscar el directorio raiz */

			dword_t primerSectorDirRaiz = primerSectorFAT+ptrpbr->bpb.sectoresPorFAT32*ptrpbr->bpb.numeroDeFATs;
			uart_puts("Primer sector directorio raiz =0x");
			uart_puts(uintToString(primerSectorDirRaiz,HEXADECIMAL));
			uart_puts("\r\n");
			/* cargo en buffer el sector del dir raiz */

			uint8_t buffer2[512];
			int k;
			for(k = 0; k < 64; k++) {
				emmc_read(bd,buffer2,512,primerSectorDirRaiz+k);
				entrada_dir_t *ptred = buffer2;

				for(i = 0; i < 16; i++) {
					if(ptred[i].nombre[0] == '\0')
					break;

					uart_puts(" nombre = ");
					int j;
					for(j=0; j<8;j++) {
						uart_putc(ptred[i].nombre[j]);
					}
					//uart_puts("\r\n");

					uart_puts(" extension = ");
					for(j=0; j<3;j++) {
						uart_putc(ptred[i].ext[j]);
					}
					//uart_puts("\r\n");

					uart_puts(" cluster = 0x");
					uart_puts(uintToString(ptred[i].clusterHigh,HEXADECIMAL));
					uart_puts(uintToString(ptred[i].clusterLow,HEXADECIMAL));
					//uart_puts("\r\n");

					uart_puts(" tamanio = 0x");
					uart_puts(uintToString(ptred[i].tam,DECIMAL));
					uart_puts("\r\n");
				}

			}

			verFich(bd,0x021c,0x1067,fat,primerSectorDirRaiz);
		}

	}
#endif

#if(0)
	uart_puts("Inicializando driver emmc......\r\n");

	struct block_device *bd = NULL;
	if (emmc_card_init(&bd) == 0) {
		uart_puts("emmc_sd_card_driver inicializado!!!!!!\r\n");
		uart_puts("block Size = ");
		uart_puts(uintToString(bd->block_size, DECIMAL));
		uart_puts("\r\n");
	} else {
		uart_puts("[ERROR] emmc_sd_card_driver NO inicializado!!!!!!\r\n");
	}
	uint8_t buffer[512];
	size_t * pointer = (size_t *) buffer;
	unsigned int i;
	for (i = 0; i < 512; ++i) {
		buffer[i] = 0;
	}
	if (emmc_read(bd, buffer, bd->block_size, 0) == -1) {
		uart_puts("[ERROR] emmc_bd error en la lectura!!!!!\r\n");
	} else {
		uart_puts("Bloque leido con exito!!!!!\r\n");
		uart_puts("Dumpeo hexadecimal del MBR: \r\n");

		for (i = 0; i < 128; ++i) {
			uart_puts(uintToString(i * 4, DECIMAL));
			uart_puts(":[");
			uart_puts(uintToString(pointer[i], HEXADECIMAL));
			uart_puts("]\r\n");
		}

		uart_puts("Vamos a leer el PBR:!!!!!\r\n");
		mbr_t * ptrmbr = (mbr_t *) buffer;

		uart_puts("SIZEOF MBR =");
		uart_puts(uintToString(sizeof(mbr_t), DECIMAL));
		uart_puts("\r\n");
		uart_puts("Primer sector de la particion =");

		uint32_t primerSector = ptrmbr->descParticion[0].primerSector;

		uart_puts(uintToString(primerSector, HEXADECIMAL));
		uart_puts("\r\n");

		if (emmc_read(bd, buffer, bd->block_size, primerSector) == -1) {
			uart_puts("[ERROR] emmc_bd error en la lectura!!!!!\r\n");
		} else {
			pbr_t * ptrpbr = (pbr_t *) buffer;
			uint32_t primerSectorFAT = primerSector
					+ ptrpbr->bpb.sectoresReservados;

			uart_puts("Sectores de la particion =");
			uart_puts(uintToString(ptrpbr->bpb.sectores, DECIMAL));
			uart_puts("\r\n");

			/* un cluster es una agruàcion de sectores contiguos, en este caso 64 */

			/* 15515648 Sectores particion / 64 Sectores/cluster = 242432 clusters particion */

			uint32_t fat[242432];

			if (emmc_card_init(&bd) == 0) {
				uart_puts("emmc_sd_card_driver re-inicializado!!!!!!\r\n");
				uart_puts("block Size = ");
				uart_puts(uintToString(bd->block_size, DECIMAL));
				uart_puts("\r\n");
			} else {
				uart_puts("[ERROR] emmc_sd_card_driver NO inicializado!!!!!!\r\n");
			}

			for (i = 0; i < 242432 / (512 / 4); i++) {
				uart_puts("Sigo en el for tio... \r\n");
				emmc_read(bd, (uint8_t *)&fat[i * (512 / 4)], 512, primerSectorFAT + i);
			}

			/* ya tenemos la FAT ahora vamos a buscar el directorio raiz */

			dword_t primerSectorDirRaiz = primerSectorFAT+ptrpbr->bpb.sectoresPorFAT32*ptrpbr->bpb.numeroDeFATs;
			uart_puts("Primer sector directorio raiz =0x");
			uart_puts(uintToString(primerSectorDirRaiz,HEXADECIMAL));
			uart_puts("\r\n");
			/* cargo en buffer el sector del dir raiz */

			uint8_t buffer2[512];
			int k;
			for(k = 0; k < 64; k++) {
				emmc_read(bd,buffer2,512,primerSectorDirRaiz+k);
				entrada_dir_t * ptred = (entrada_dir_t *) buffer2;

				for(i = 0; i < 16; i++) {
					if(ptred[i].nombre[0] == '\0')
					break;

					uart_puts("primer car: ");
					printByte(ptred[i].nombre[0]);

					uart_puts(" nombre = ");
					int j;
					for(j=0; j<8;j++) {
						uart_putc(ptred[i].nombre[j]);
					}

					//uart_puts("\r\n");

					uart_puts(" extension = ");
					for(j=0; j<3;j++) {
						uart_putc(ptred[i].ext[j]);
					}
					//uart_puts("\r\n");

					uart_puts(" cluster = 0x");
					uint32_t cluster = (((uint32_t) ptred[i].clusterHigh) << 16) + ptred[i].clusterLow;
					//uart_puts(uintToString(ptred[i].clusterHigh,HEXADECIMAL));
					//uart_puts(uintToString(ptred[i].clusterLow,HEXADECIMAL));
					uart_puts(uintToString(cluster,HEXADECIMAL));
					//uart_puts("\r\n");

					uart_puts(" tamanio = 0x");
					uart_puts(uintToString(ptred[i].tam,DECIMAL));
					uart_puts("\r\n");
				}

			}

			//init_vmem();
			init_pmem();
			init_syscalls();
			uart_puts("inits have been done!\r\n");

			create_main_process();

			//kfork("Sample process 1", (Dir_t) &sample_process_1);
			//kfork("Sample process 2", (Dir_t) &sample_process_2);


			bd = NULL;
			if (emmc_card_init(&bd) != 0) {
				uart_puts("[ERROR] emmc_sd_card_driver NO inicializado!!!!!!\r\n");
			}

			/* up5 */
			//prgm2proc(bd, 0x358, 633, fat, primerSectorDirRaiz);
			/* up5b */
			prgm2proc(bd, 0x39C, 822, fat, primerSectorDirRaiz);

			uart_puts("PROCESO CARGADO!\r\n");
			uart_puts("\r\n\r\n\r\n\r\n");
			//RPI_esperarMicroSeconds(1000000);

			/* print marko */

			uart_puts("primera instruccion: ");
			unsigned int instruct = *((unsigned int * ) (0x00101000 + 54));

			uart_puts(uintToString(instruct,HEXADECIMAL));

			//asm volatile("MOV PC, %[addr]" : : [addr] "r" (0x00104000 + 54) );
		}
	}



	/* Enable the timer interrupt IRQ */
	RPI_GetIrqController()->Enable_Basic_IRQs = RPI_BASIC_ARM_TIMER_IRQ;

	RPI_GetIrqController()->Disable_IRQs_1 = 0xFFFFFFFF;
	RPI_GetIrqController()->Disable_IRQs_2 = 0xFFFFFFFF;

	RPI_GetIrqController()->Enable_IRQs_2 = 0x02000000;

	/* Setup the system timer interrupt */
	/* Timer frequency = Clk/256 * 0x400 */
	RPI_GetArmTimer()->Load = 0x400;

	/* Setup the ARM Timer */
	RPI_GetArmTimer()->Control =
	RPI_ARMTIMER_CTRL_23BIT |

	RPI_ARMTIMER_CTRL_ENABLE |
	RPI_ARMTIMER_CTRL_INT_ENABLE |
	RPI_ARMTIMER_CTRL_PRESCALE_256;

	uart_puts("Comienza roundrobin!\r\n");
	/* Enable interrupts! */
	_enable_interrupts();
	asm volatile("cps #0x10");

	while (1) {
		//uart_puts("Main while!\r\n");
	}
#endif

#if(1)

	bgInit(atagsAddr);
	init_syscalls();
	create_main_process();
	init_uartConsole();

	/* Enable the timer interrupt IRQ */
	RPI_GetIrqController()->Enable_Basic_IRQs = RPI_BASIC_ARM_TIMER_IRQ;

	/* Setup the system time r interrupt */
	/* Timer frequency = Clk/256 * 0x400 */
	RPI_GetArmTimer()->Load = 0x800;

	/* Setup the ARM Timer */
	RPI_GetArmTimer()->Control =
	RPI_ARMTIMER_CTRL_23BIT |
	RPI_ARMTIMER_CTRL_ENABLE |
	RPI_ARMTIMER_CTRL_INT_ENABLE |
	RPI_ARMTIMER_CTRL_PRESCALE_256;

	//Enable uart interrupts:

	RPI_GetIrqController()->Disable_IRQs_1 = 0xFFFFFFFF;
	RPI_GetIrqController()->Disable_IRQs_2 = 0xFFFFFFFF;

	RPI_GetIrqController()->Enable_IRQs_2 = 0x02000000;

	uart_puts("creando procs de ejemplo!\r\n");
/*
	kfork("Sample process 1", (Dir_t) &sample_process_1, (Dir_t) 0x1f000000);
	uart_puts("KFORK 1 HECHO, resultado de la pila:\r\n");
	printMemDump(0x1f000000 - 16*4,16);
*/
	kfork("Uart_Console", (Dir_t) &activateUartConsole, (Dir_t) 0x1e000000);
	uart_puts("KFORK 2 HECHO, resultado de la pila:\r\n");
	printMemDump(0x1e000000 - 16*4,16);

	asm volatile("cpsie i,#0x10");
	while(1){
		uart_puts("while...\n\r");
		RPI_esperarMicroSeconds(1000000);
	}

#endif

	return 0;
}
