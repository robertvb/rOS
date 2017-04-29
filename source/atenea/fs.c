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

#include "../includes/atenea/fs.h"

void printByte(uint8_t value) {
	char buffer[17] = "0123456789ABCDEF";
	uart_putc(buffer[value>>4]);
	uart_putc(buffer[value & 0x0F]);
}

void prgm2proc(struct block_device * bd, unsigned int cluster,
		unsigned int fichTam, uint32_t * fat, unsigned int primerSectorDirRaiz) {

	/* Paso 1: obtenemos el pid del proceso */

	Pid_t pid = getNextPid();

	/* Paso 2: reservamos memoria para el proceso */

	Dir_t pdir = instance_process(pid,fichTam);

	/* Si no hay memoria suficiente, se retorna */
	if(pdir == NULL) {
		return;
	}

	/* Paso 3: Comenzamos la carga del fichero en sus marcos correspondientes */

	uart_puts("Cargando fichero en memoria: ");
	/* convertir cluster en sector */
	uint8_t buffer[512];
	unsigned int sect;

	sect = primerSectorDirRaiz + 64 * (cluster - 2); /* porque el primer cluster es el 2!!!!!!!!!!!!! */
	uart_puts(" sect ----> ");
	uart_puts(uintToString(sect,HEXADECIMAL));
	uart_puts(" primersectRaiz ----> ");
	uart_puts(uintToString(primerSectorDirRaiz,HEXADECIMAL));
	uart_puts(" cluster ----> ");
	uart_puts(uintToString(cluster,HEXADECIMAL));
	int cont = fichTam;
	uint8_t * pointer = buffer;
	do{
		int i,j;
		for(i = 0; i < 64;i++) {
			/* leer los sectores del fixero */
    		uart_puts("LEYENDO SECTOR: ");
    		uart_puts(uintToString(sect,HEXADECIMAL));
    		uart_puts(" \r\n");
			if( emmc_read(bd,buffer,512,sect++)== -1) {
	        	uart_puts("[ERROR] emmc_bd error en la lectura!!!!!\r\n");
	        	return;
	        }
    		uart_puts("sector leido!\n\r");
			/* Cargamos en memoria el sector */
	        for (j = 0; j < 512; ++j) {
	        	uint8_t * ldir = (uint8_t *) pdir;
	        	ldir[i*512 + j] = pointer[j];
	        	printByte( pointer[j]);
	        	uart_puts(" ");
	    	}
		cont-=512;
		if(cont <= 0)
			break;
		}
		cluster = fat[cluster];
		sect = primerSectorDirRaiz + 64 * (cluster - 2);
	}while(cont > 0);

	/* Paso 4: forkeamos */

	uart_puts("\n\rDireccion pdir + ELfHdr: ");
	uart_puts(uintToString((unsigned int) pdir + sizeof(Elf32Hdr_t),HEXADECIMAL));

	kfork("sp1 test",(unsigned int) pdir + sizeof(Elf32Hdr_t));

}

void verFich(struct block_device * bd, unsigned int cluster,
		unsigned int fichTam, uint32_t * fat, unsigned int primerSectorDirRaiz) {

	/* convertir cluster en sector */
	uint8_t buffer[512];
	unsigned int sect;

	sect = primerSectorDirRaiz + 64 * (cluster - 2); /* porque el primer cluster es el 2!!!!!!!!!!!!! */
	int cont = fichTam;
	uint8_t * pointer = buffer;
	do{
		int i,j;
		for(i = 0; i < 64;i++) {
			/* leer los sectores del fixero */
    		uart_puts("LEYENDO SECTOR: ");
    		uart_puts(uintToString(sect,HEXADECIMAL));
    		uart_puts(" \r\n");
			if( emmc_read(bd,buffer,512,sect++)== -1) {
	        	uart_puts("[ERROR] emmc_bd error en la lectura!!!!!\r\n");
	        	return;
	        }
    		uart_puts("sector leido!\n\r");
			/*pintar el sector leido */
	        for (j = 0; j < 512; ++j) {
	    		//uart_puts(uintToString(pointer[j],HEXADECIMAL));
	        	printByte(pointer[j]);
	    		uart_puts(" ");
	    		if(j%40==0) {
		    		uart_puts("\r\n");
	    		}
	    	}
		cont-=512;
		if(cont <= 0)
			break;
		}
		cluster = fat[cluster];
		sect = primerSectorDirRaiz + 64 * (cluster - 2);
	}while(cont > 0);

}
