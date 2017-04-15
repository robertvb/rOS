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

#ifndef SOURCE_INCLUDES_ATENEA_ELF32_H_
#define SOURCE_INCLUDES_ATENEA_ELF32_H_

/* Cabecera ELF */

typedef struct {			/********************FORMATO CABECERA ELF********************/
  uint8_t  e_ident[16];		/* identidad. Para ver si efectivamente se trata de un ELF.	*/
  uint16_t e_type;      	/* 1 = relocatable. 2 = executable 3 = shared 4 = core.		*/
  uint16_t e_machine;   	/* Maquina. 0x28 = ARM.										*/
  uint32_t e_version;		/* Version del ELF. 										*/
  uint32_t e_entry;     	/* Direccion de memoria desde la que se empieza a ejecutar. */
  uint32_t e_phoff;     	/* Punto de entrada de la PHT (Program header table).       */
  uint32_t e_shoff;			/* Punto de entrada de la SHT (Section header table).		*/
  uint32_t e_flags;			/* FLAGS. 													*/
  uint16_t e_ehsize;		/* Tamanio de esta cabecera. En este caso 52 Bytes.			*/
  uint16_t e_phentsize;		/* Tamanio de la PHT.										*/
  uint16_t e_phnum;			/* Numero de la entradas de la PHT. 						*/
  uint16_t e_shentsize;		/* Tamanio de la SHT.										*/
  uint16_t e_shnum;			/* Numero de la entradas de la SHT. 						*/
  uint16_t e_shstrndx;		/* Indice de la SHT que contiene los nombres de secciones.  */
} Elf32Hdr_t;					/************************************************************/

/* Program Header */

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_Phdr_t;

/* Section Header */

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
} Elf32_Shdr_t;

#endif /* SOURCE_INCLUDES_ATENEA_ELF32_H_ */
