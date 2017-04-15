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

/* Thank to Pedro Pablo (GITHUB: TODO) */

#ifndef SOURCE_ATENEA_MBR_H_
#define SOURCE_ATENEA_MBR_H_

/* entrada de la tabla de particiones */
typedef uint8_t byte_t;
typedef uint32_t dword_t;
typedef uint16_t word_t;

typedef struct                                          /* descParticion_t */
{
    byte_t  activa ;                                          /*   1 bytes */
    byte_t  cabezaInicial ;                /* H1 */ /* CHS */ /*   1 bytes */
    byte_t  sectorInicial ;                /* S1 */           /*   1 bytes */
    byte_t  cilindroInicial ;              /* C1 */           /*   1 bytes */
    byte_t  tipo ;                                            /*   1 bytes */
    byte_t  cabezaFinal ;                  /* H2 */ /* CHS */ /*   1 bytes */
    byte_t  sectorFinal ;                  /* S2 */           /*   1 bytes */
    byte_t  cilindroFinal ;                /* C2 */           /*   1 bytes */
    dword_t primerSector ;                          /* LBA */ /*   4 bytes */
    dword_t sectores ;                                        /*   4 bytes */
} descParticion_t ;                                    /*  total: 16 bytes */

#define maxParticiones 4  /* numero de entradas en la tabla de particiones */

#define tamMBR       512                                          /* 0x200 */

#define tamCodigoMBR 440                                          /* 0x1B8 */

#define partes_del_MBR \
    byte_t codigo [ tamCodigoMBR ] ;                          /* 440 bytes */\
    dword_t WinDiskSig ;                                      /*   4 bytes */\
    word_t relleno ;                                          /*   2 bytes */\
    descParticion_t descParticion [ maxParticiones ] ;        /*  64 bytes */\
    word_t signatura ;                                        /*   2 bytes */\

#pragma pack(1)
typedef struct                                /* master boot record: mbr_t */
{
    partes_del_MBR
} mbr_t ;
#pragma pop

#endif /* SOURCE_HADES_MBR_H_ */
