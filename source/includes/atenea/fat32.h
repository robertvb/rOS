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

#ifndef SOURCE_INCLUDES_ATENEA_BPB_H_
#define SOURCE_INCLUDES_ATENEA_BPB_H_

/* Thank to Pedro Pablo (GITHUB: TODO) */

/* MBR Master boot Record */

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

/* BPB = Bloque de parametros del BIos */
#define camposBPB( xtrn ) \
xtrn byte_t  instJMP [2] ;          /* instruccion JMP (0xEB, 0x3C)        */\
xtrn byte_t  instNOP ;              /* instruccion NOP (0x90) de relleno   */\
xtrn byte_t  OEM [8] ;              /* OEM, 8 caracteres                   */\
xtrn word_t  bytesPorSector ;       /* bytes por sector                    */\
xtrn byte_t  sectoresPorCluster ;   /* sectores por cluster                */\
xtrn word_t  sectoresReservados ;   /* sectores reservados                 */\
xtrn byte_t  numeroDeFATs ;         /* numero de FATs                      */\
xtrn word_t  entradasDirRaiz ;      /* entradas del directorio raiz        */\
xtrn word_t  sectores16 ;           /* numero de sectores en total (16 bts)*/\
xtrn byte_t  tipoDeMedio ;          /* descriptor de medio                 */\
xtrn word_t  sectoresPorFAT ;       /* sectores pot FAT                    */\
xtrn word_t  sectoresPorPista ;     /* sectores por pista                  */\
xtrn word_t  cabezas ;              /* cabezas                             */\
xtrn dword_t primerSector ;         /* primer sector de la particion       */\
xtrn dword_t sectores ;             /* numero de sectores en total (32 bts)*/\
xtrn dword_t sectoresPorFAT32 ;     /* sectores por fat32                  */\
xtrn word_t  flags;                   /* flags 								*/\
xtrn word_t  fs_version  ;            /* */ \
xtrn dword_t root_base_cluster ;\
xtrn byte_t  unidad ;               /* unidad (0 -> A:)                    */\
xtrn byte_t  relleno ;              /* byte que no se usa                  */\
xtrn byte_t  signatura ;            /* extension de la signatura           */\
xtrn dword_t numeroDeSerie ;        /* numero de serie                     */\
xtrn char    etiqueta [11]  ;       /* etiqueta de volumen (11 caracteres) */\
xtrn char    SF [8] ;               /* tipo de sistema de ficheros         */

//camposBPB( extern ) ;  /* xtrn = extern, static, register, auto o volatile */

#pragma pack(1)
typedef struct {
  camposBPB(volatile)
} bpb_t ;
#pragma pop

#define tamBPB 0x003E

#pragma pack(1)
typedef struct {

bpb_t bpb;
uint8_t relleno[512-tamBPB-2];
uint16_t signatura;

}pbr_t;
#pragma pop

#pragma pack(1)
typedef struct {
	uint8_t nombre[8];
	uint8_t ext[3];
	uint8_t relleno[9];
	uint16_t clusterHigh;
	uint32_t relleno2;
	uint16_t clusterLow;
	uint32_t tam;
}entrada_dir_t;
#pragma pop

#endif /* SOURCE_INCLUDES_ATENEA_FAT32_H_ */
