.section .text.startup

.globl _start
.global _get_stack_pointer
.global _exception_table
.global _enable_interrupts
.global _disable_interrupts
.global _main_endloop

/* From the ARM ARM (Architecture Reference Manual). Make sure you get the
 ARMv5 documentation which includes the ARMv6 documentation which is the
 correct processor type for the Broadcom BCM2835. The ARMv6-M manuals
 available on the ARM website are for Cortex-M parts only and are very
 different.

 See ARM section A2.2 (Processor Modes)
*/

.equ    CPSR_MODE_USER,         0x10
.equ    CPSR_MODE_FIQ,          0x11
.equ    CPSR_MODE_IRQ,          0x12
.equ    CPSR_MODE_SVR,          0x13
.equ    CPSR_MODE_ABORT,        0x17
.equ    CPSR_MODE_UNDEFINED,    0x1B
.equ    CPSR_MODE_SYSTEM,       0x1F
/* See ARM section A2.5 (Program status registers)*/
.equ    CPSR_IRQ_INHIBIT,       0x80
.equ    CPSR_FIQ_INHIBIT,       0x40
.equ    CPSR_THUMB,             0x20

_start:

    ldr pc, _reset_h
    ldr pc, _undefined_instruction_vector_h
    ldr pc, _software_interrupt_vector_h
    ldr pc, _prefetch_abort_vector_h
    ldr pc, _data_abort_vector_h
    ldr pc, _unused_handler_h
    ldr pc, _interrupt_vector_h
    ldr pc, _fast_interrupt_vector_h

_reset_h:                           .word   _reset_
_undefined_instruction_vector_h:    .word   undefined_instruction_vector
_software_interrupt_vector_h:       .word   software_interrupt_vector
_prefetch_abort_vector_h:           .word   prefetch_abort_vector
_data_abort_vector_h:               .word   data_abort_vector
_unused_handler_h:                  .word   _reset_
_interrupt_vector_h:                .word   interrupt_vector
_fast_interrupt_vector_h:           .word   fast_interrupt_vector

_reset_:
    /* We enter execution in supervisor mode. For more information on
     processor modes see ARM Section A2.2 (Processor Modes)

     salvamos r0,r1 y atagsAddr. */
    mov r10, r0
    mov r11, r1
    mov r12, r2

    mov     r0, #0x8000
    mov     r1, #0x0000
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}

    /* We're going to use interrupt mode, so setup the interrupt mode
     stack pointer which differs to the application stack pointer: */
    mov r0, #(CPSR_MODE_IRQ | CPSR_IRQ_INHIBIT | CPSR_FIQ_INHIBIT )
    msr cpsr_c, r0
    mov sp, #(0x00100000)

    /* Switch back to supervisor mode (our application mode) and
     set the stack pointer towards the end of RAM. Remember that the
     stack works its way down memory, our heap will work it's way
     up memory toward the application stack. */
    mov r0, #(CPSR_MODE_SVR | CPSR_IRQ_INHIBIT | CPSR_FIQ_INHIBIT )
    msr cpsr_c, r0

    /* Set the stack pointer at some point in RAM that won't harm us
     It's different from the IRQ stack pointer above and no matter
     what the GPU/CPU memory split, 64MB is available to the CPU
     Keep it within the limits and also keep it aligned to a 32-bit
     boundary! */
    mov     sp, #(0x00ff000)

    /* The c-startup function which we never return from. This function will
     initialise the ro data section (most things that have the const
     declaration) and initialise the bss section variables to 0 (generally
     known as automatics). It'll then call main, which should never return.*/

mov r0,#1024
mov r1,#768
mov r2,#16
bl InitialiseFrameBuffer


/* recuperamos r1,r2 y atagsAddr */
mov r0, r10
mov r1, r11
mov r2, r12

/* llamada a main */
bl main

    /* If main does return for some reason, just catch it and stay here. */
_main_endloop:
    b       _main_endloop


_get_stack_pointer:
    /* Return the stack pointer value */
    str     sp, [sp]
    ldr     r0, [sp]

    /* Return from the function */
    mov     pc, lr


_enable_interrupts:
    mrs     r0, cpsr
    bic     r0, r0, #0x80
    msr     cpsr_c, r0

    mov     pc, lr

_disable_interrupts:
    mrs		r0, CPSR
    orr		r1, r0, #0x80
    msr		CPSR_c, r1

	mov     pc, lr

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
