
#include "../includes/poseidon/rpi-interrupts.h"

#include <stdint.h>
#include <stdbool.h>

#include "../includes/poseidon/rpi-armtimer.h"
#include "../includes/zeus/scheduler.h"

extern void _main_endloop();

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
	register unsigned int addr, far;
	asm volatile("mov %[addr], lr" : [addr] "=r" (addr) );
	/* Read fault address register */
	asm volatile("mrc p15, 0, %[addr], c6, c0, 0": [addr] "=r" (far) );


	uart_puts("Data abort! (RESET VECTOR)\n");
	uart_puts("Instruction address: 0x");
	/* addr = lr, but the very start of the abort routine does
	 * sub lr, lr, #4
	 * lr = address of aborted instruction, plus 8
	 */
    uart_puts(uintToString(addr,HEXADECIMAL));
	uart_puts("\n\r");

	uart_puts("  fault address: 0x");
    uart_puts(uintToString(far,HEXADECIMAL));
	uart_puts("\n\r");
}

/**
    @brief The undefined instruction interrupt handler

    If an undefined intstruction is encountered, the CPU will start
    executing this function. Just trap here as a debug solution.
*/
void __attribute__((interrupt("UNDEF"))) undefined_instruction_vector(void)
{
	uart_puts("                               ________________\n\r");
	uart_puts("                          ____/ (  (    )   )  \\___\n\r");
	uart_puts("                         /( (  (  )   _    ))  )   )\\\n\r");
	uart_puts("                       ((     (   )(    )  )   (   )  )\n\r");
	uart_puts("                     ((/  ( _(   )   (   _) ) (  () )  )\n\r");
	uart_puts("                    ( (  ( (_)   ((    (   )  .((_ ) .  )_\n\r");
	uart_puts("                   ( (  )    (      (  )    )   ) . ) (   )\n\r");
	uart_puts("                  (  (   (  (   ) (  _  ( _) ).  ) . ) ) ( )\n\r");
	uart_puts("                  ( (  (   ) (  )   (  ))     ) _)(   )  )  )\n\r");
	uart_puts("                 ( (  ( \\ ) (    (_  ( ) ( )  )   ) )  )) ( )\n\r");
	uart_puts("                 (  (   (  (   (_ ( ) ( _    )  ) (  )  )   )\n\r");
	uart_puts("                 ( (  ( (  (  )     (_  )  ) )  _)   ) _( ( )\n\r");
	uart_puts("                  ((  (   )(    (     _    )   _) _(_ (  (_ )\n\r");
	uart_puts("                   (_((__(_(__(( ( ( |  ) ) ) )_))__))_)___)\n\r");
	uart_puts("                  ((__)        \\\\||lll|l||///          \\_))\n\r");
	uart_puts("                            (   /(/ (  )  ) )\\   )\n\r");
	uart_puts("                         (    ( ( ( | | ) ) )\\   )\n\r");
	uart_puts("                            (   /(| / ( )) ) ) )) )\n\r");
	uart_puts("                         (     ( ((((_(|)_)))))     )\n\r");
	uart_puts("                          (      ||\\(|(|)|/||     )\n\r");
	uart_puts("                        (        |(||(||)||||        )\n\r");
	uart_puts("                          (     //|/l|||)|\\\\ \\     )\n\r");
	uart_puts("                        (/ / //  /|//||||\\\\  \\ \\  \\ _)\n\r");
	uart_puts("--------------------- >>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<< ------------------\n\r");
	uart_puts("---------------------- >>>>>>> rOS-KERNEL PANIC <<<<<<<<< -------------------\n\r");
	uart_puts("--------------------- >>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<< ------------------\n\r");

	uart_puts("[ERROR] UNDEF INTERRUPT");
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
	unsigned int arg0;

	// save registers
	//asm volatile("push {r0-r12}");

	// Read link register into addr - contains the address of the instruction after the SWI
	asm volatile("mov %[addr], lr" : [addr] "=r" (addr) );

	/* cargamos un parametro */
	asm volatile("mov %[arg0], r0" : [arg0] "=r" (arg0) );

	uart_puts("Valor de lr en poseidon: 0x");
    uart_puts(uintToString(addr,HEXADECIMAL));
	uart_puts("\n\r");

	uart_puts("Valor de arg0 en poseidon: 0x");
    uart_puts(uintToString(arg0,HEXADECIMAL));
	uart_puts("\n\r");

	uart_puts("String de arg0: \"");
	uint8_t i;
	for(i = 0; ((uint8_t *) arg0)[i] != 0; i++ ) {
		printByte(((uint8_t *) arg0)[i]);
	}
	uart_puts("\"\n\r");

	uart_puts("String de 0x101084");
    uart_puts((unsigned char*) 0x101084);
	uart_puts("\n\r");

	// Bottom 24 bits of the SWI instruction are the SWI number
	swi = *((unsigned int *)(addr - 4)) & 0x00ffffff;

	// Changing processor mode to handle the syscall
	asm volatile("cps #0x1f");

	// Handle syscall
	syscall(swi,addr,arg0);

	uart_puts("Turning interrupt on again");
	uart_puts("\n\r");

	// pop de los registros del proceso
	//asm volatile("pop {r0-r12}");

	// Se rehabilitan las interrupciones
	asm volatile("cpsie i");

	asm volatile("MOV PC, %[Addr]" : : [Addr] "r" (addr) );
}


/**
    @brief The prefetch abort interrupt handler

    The CPU will start executing this function. Just trap here as a debug
    solution.
*/
void __attribute__((interrupt("ABORT"))) prefetch_abort_vector(void)
{
	register unsigned int addr;
	asm volatile("mov %[addr], lr" : [addr] "=r" (addr) );

	uart_puts("[ERROR] Prefetch abort!!!!!!!!\n\r");
	uart_puts("Instruction address: 0x");
	/* lr = address of aborted instruction, plus 4
	 * addr = lr, but the very start of the abort routine does
	 * sub lr, lr, #4
	 */
    uart_puts(uintToString(addr,HEXADECIMAL));
	uart_puts("\n\r");

	uart_puts("                               ________________\n\r");
	uart_puts("                          ____/ (  (    )   )  \\___\n\r");
	uart_puts("                         /( (  (  )   _    ))  )   )\\\n\r");
	uart_puts("                       ((     (   )(    )  )   (   )  )\n\r");
	uart_puts("                     ((/  ( _(   )   (   _) ) (  () )  )\n\r");
	uart_puts("                    ( (  ( (_)   ((    (   )  .((_ ) .  )_\n\r");
	uart_puts("                   ( (  )    (      (  )    )   ) . ) (   )\n\r");
	uart_puts("                  (  (   (  (   ) (  _  ( _) ).  ) . ) ) ( )\n\r");
	uart_puts("                  ( (  (   ) (  )   (  ))     ) _)(   )  )  )\n\r");
	uart_puts("                 ( (  ( \\ ) (    (_  ( ) ( )  )   ) )  )) ( )\n\r");
	uart_puts("                 (  (   (  (   (_ ( ) ( _    )  ) (  )  )   )\n\r");
	uart_puts("                 ( (  ( (  (  )     (_  )  ) )  _)   ) _( ( )\n\r");
	uart_puts("                  ((  (   )(    (     _    )   _) _(_ (  (_ )\n\r");
	uart_puts("                   (_((__(_(__(( ( ( |  ) ) ) )_))__))_)___)\n\r");
	uart_puts("                  ((__)        \\\\||lll|l||///          \\_))\n\r");
	uart_puts("                            (   /(/ (  )  ) )\\   )\n\r");
	uart_puts("                         (    ( ( ( | | ) ) )\\   )\n\r");
	uart_puts("                            (   /(| / ( )) ) ) )) )\n\r");
	uart_puts("                         (     ( ((((_(|)_)))))     )\n\r");
	uart_puts("                          (      ||\\(|(|)|/||     )\n\r");
	uart_puts("                        (        |(||(||)||||        )\n\r");
	uart_puts("                          (     //|/l|||)|\\\\ \\     )\n\r");
	uart_puts("                        (/ / //  /|//||||\\\\  \\ \\  \\ _)\n\r");
	uart_puts("--------------------- >>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<< ------------------\n\r");
	uart_puts("---------------------- >>>>>>> rOS-KERNEL PANIC <<<<<<<<< -------------------\n\r");
	uart_puts("--------------------- >>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<< ------------------\n\r");
	/* Set the return address to be the function main_endloop(), by
	 * putting its address into the program counter
	 *
	 * THIS IS JUST A TEST - you can't normally do this as it doesn't
	 * restore the registers or put the stack pointer back where it was,
	 * so repeated aborts will overflow the stack.
	 */
	uart_puts("JUMPING TO END LOOP\n\r");
	asm volatile("movs pc, %[addr]" : :
		[addr] "r" ((unsigned int)(&_main_endloop)) );

	/* Doesn't reach this point */
}


/*
 * Faltas de pagina
 */
void __attribute__((interrupt("ABORT"))) data_abort_vector(void)
{
	register unsigned int addr, far;
	asm volatile("mov %[addr], lr" : [addr] "=r" (addr) );
	/* Read fault address register */
	asm volatile("mrc p15, 0, %[addr], c6, c0, 0": [addr] "=r" (far) );

	uart_puts("[SWI] Data abort! (Falta de pagina): ");
	uart_puts("Instruction address: 0x");
	/* addr = lr, but the very start of the abort routine does
	 * sub lr, lr, #4
	 * lr = address of aborted instruction, plus 8
	 */
    uart_puts(uintToString(addr,HEXADECIMAL));

	uart_puts("  fault address: 0x");
    uart_puts(uintToString(far,HEXADECIMAL));
	uart_puts("\n\r");

	/* Routine terminates by returning to LR-4, which is the instruction
	 * after the aborted one
	 * GCC doesn't properly deal with data aborts in its interrupt
	 * handling - no option to return to the failed instruction
	 */

	asm volatile("cps #0x1f");
	asm volatile("cpsid aif");
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
	uart_puts("[ERROR] FIQ INTERRUPT");
}

void timer_reset(void)
{
    RPI_GetArmTimer()->IRQClear = 0;
    *armTimerIRQClear = 0;
}
