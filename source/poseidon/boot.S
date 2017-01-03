.section .text.startup

.globl _start
_start:

push {r0,r1,r2}

mov r0,#1024
mov r1,#768
mov r2,#16
bl InitialiseFrameBuffer

pop {r0,r1,r2}

bl main

# NUNCA se debe retornar de main, en caso de retorno el sistema queda atrapado en _infinite_loop$:

_infinite_loop$:
b _infinite_loop$

.section .data

.globl BackgroundInfo
BackgroundInfo:
	.int 0
	.int 0
	.int 0
	.int 0
	.int 0
	.int 0

.align 12
.globl FrameBufferInfo 
FrameBufferInfo:
	.int 1024	/* #0 Width */
	.int 768	/* #4 Height */
	.int 1024	/* #8 vWidth */
	.int 768	/* #12 vHeight */
	.int 0		/* #16 GPU - Pitch */
	.int 16		/* #20 Bit Dpeth */
	.int 0		/* #24 X */
	.int 0		/* #28 Y */
	.int 0		/* #32 GPU - Pointer */
	.int 0		/* #36 GPU - Size */

.align 4
.globl SystemDefaultFontInfo 
SystemDefaultFontInfo:

.incbin "font.bin" /* Monospace, Monochrome 8x16 */
