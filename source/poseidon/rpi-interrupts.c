
#include "../includes/poseidon/rpi-interrupts.h"

#include <stdint.h>
#include <stdbool.h>

#include "../includes/poseidon/rpi-armtimer.h"
#include "../includes/zeus/scheduler.h"

/** @brief The BCM2835 Interupt controller peripheral at it's base address */
static rpi_irq_controller_t* rpiIRQController =
        (rpi_irq_controller_t*)RPI_INTERRUPT_CONTROLLER_BASE;

static volatile unsigned int *armTimerIRQClear = (unsigned int *) 0x2000b40c;
/**
    @brief Return the IRQ Controller register set
*/
rpi_irq_controller_t* RPI_GetIrqController( void )
{
    return rpiIRQController;
}


/**
    @brief The Reset vector interrupt handler

    This can never be called, since an ARM core reset would also reset the
    GPU and therefore cause the GPU to start running code again until
    the ARM is handed control at the end of boot loading
*/
void __attribute__((interrupt("ABORT"))) reset_vector(void)
{

}

/**
    @brief The undefined instruction interrupt handler

    If an undefined intstruction is encountered, the CPU will start
    executing this function. Just trap here as a debug solution.
*/
void __attribute__((interrupt("UNDEF"))) undefined_instruction_vector(void)
{
    while( 1 )
    {
        /* Do Nothing! */
    }
}


/**
    @brief The supervisor call interrupt handler

    The CPU will start executing this function. Just trap here as a debug
    solution.
*/
// Software interrupt
__attribute__ ((interrupt ("SWI"))) void software_interrupt_vector(void)
{
	unsigned int addr;
	unsigned int swi;

	// Read link register into addr - contains the address of the instruction after the SWI
	asm volatile("mov %[addr], lr" : [addr] "=r" (addr) );

	addr -= 4;

	// Bottom 24 bits of the SWI instruction are the SWI number
	swi = *((unsigned int *)addr) & 0x00ffffff;

	// Changing processor mode
	asm volatile("cps #0x1f");

	// Handle syscall
	syscall(swi);
}


/**
    @brief The prefetch abort interrupt handler

    The CPU will start executing this function. Just trap here as a debug
    solution.
*/
void __attribute__((interrupt("ABORT"))) prefetch_abort_vector(void)
{

}


/**
    @brief The Data Abort interrupt handler

    The CPU will start executing this function. Just trap here as a debug
    solution.
*/
void __attribute__((interrupt("ABORT"))) data_abort_vector(void)
{

}


/**
    @brief The IRQ Interrupt handler

    This handler is run every time an interrupt source is triggered. It's
    up to the handler to determine the source of the interrupt and most
    importantly clear the interrupt flag so that the interrupt won't
    immediately put us back into the start of the handler again.
*/
// IRQ
__attribute__ ((interrupt ("IRQ"))) void interrupt_vector(void)
{
	// Esta rutina empieza en modo IRQ

	// Se guardan todos los registros en la IRQ STACK (R13)
	asm volatile("push {R0-R12}");

	// Put LR del IRQ mode (Que es el PC del proceso interrumpido) en R0
	asm volatile("MOV R0, LR");


	// Cambio de modo a System mode
	asm volatile("cps #0x1f");

	// Push R0 (PC del proceso interrumpido) a la pila del system mode
	asm volatile("push {R0}");


	// Return to IRQ mode
	asm volatile("cps #0x12");

	// Pop all registers again
	asm volatile("pop {R0-R12}");


	// Return to system mode
	asm volatile("cps #0x1f");

	// Push todos los registeos pero esta vez dentro de la system mode stack
	asm volatile("push {R0-R12}");

	// Push de LR del proceso interrumpido a la system mode stack
	asm volatile("push {LR}");

	// Copy the Saved Program Status Register to R0
	// http://lioncash.github.io/ARMBook/the_apsr,_cpsr,_and_the_difference_between_them.html
    asm volatile("MRS R0, SPSR");

    // Se guarda el SPSR en la system mode stack
    asm volatile("push {R0}");


    // Return to IRQ mode
    asm volatile("cps #0x12");

	// Copy LR to R0
	asm volatile("MOV R0, LR");

	// Back to system mode
	asm volatile("cps #0x1f");

	unsigned int pc;

    unsigned int stack_pointer;

	// PC y SP
	asm volatile ("MOV %0, R0\n\t" : "=r" (pc) );
    asm volatile ("MOV %0, SP\n\t" : "=r" (stack_pointer) );

	// Se invoca al dispatcher para el cambio de contexto
	schedule_timeout(stack_pointer, pc);
}


/**
    @brief The FIQ Interrupt Handler

    The FIQ handler can only be allocated to one interrupt source. The FIQ has
    a full CPU shadow register set. Upon entry to this function the CPU
    switches to the shadow register set so that there is no need to save
    registers before using them in the interrupt.

    In C you can't see the difference between the IRQ and the FIQ interrupt
    handlers except for the FIQ knowing it's source of interrupt as there can
    only be one source, but the prologue and epilogue code is quite different.
    It's much faster on the FIQ interrupt handler.

    The prologue is the code that the compiler inserts at the start of the
    function, if you like, think of the opening curly brace of the function as
    being the prologue code. For the FIQ interrupt handler this is nearly
    empty because the CPU has switched to a fresh set of registers, there's
    nothing we need to save.

    The epilogue is the code that the compiler inserts at the end of the
    function, if you like, think of the closing curly brace of the function as
    being the epilogue code. For the FIQ interrupt handler this is nearly
    empty because the CPU has switched to a fresh set of registers and so has
    not altered the main set of registers.
*/
void __attribute__((interrupt("FIQ"))) fast_interrupt_vector(void)
{

}

void timer_reset(void)
{
    RPI_GetArmTimer()->IRQClear = 0;
    *armTimerIRQClear = 0;
}
