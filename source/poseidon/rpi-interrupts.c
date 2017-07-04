#include "../includes/poseidon/rpi-interrupts.h"

#include <stdint.h>
#include <stdbool.h>

#include "../includes/poseidon/rpi-armtimer.h"
#include "../includes/hades/rpi-uart.h"
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

void timer_reset(void)
{
    RPI_GetArmTimer()->IRQClear = 0;
    *armTimerIRQClear = 0;
}

static void __attribute__ ((naked)) _force_to_save_new_stack(unsigned int new_stack) {
	asm volatile("mov pc, lr");
}

static void print_kernel_panic(void) {

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

	return;
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
	asm volatile("cpsid i");
	register unsigned int addr;
	asm volatile("mov %[addr], lr" : [addr] "=r" (addr) );

	print_kernel_panic();

	uart_puts("[ERROR] UNDEFINED INSTRUCCION AT ");
    uart_puts(uintToString(addr,HEXADECIMAL));
	uart_puts("\n\r");

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
void __attribute__ ((naked)) software_interrupt_vector(void) {
	// En esta funcion se entra en modo supervidor (SVR)

	volatile unsigned int swi;
	volatile unsigned int lr_addr;
	volatile unsigned int sp_addr;
	volatile unsigned int spsr;
	volatile unsigned int new_stack;

	//uart_puts("INICIO SW RTI\n\r");

	// PUSH PC y SPSR en la pila del proceso interrumpido.
	asm volatile("SRSDB #16!");

	// Cambio a modo SYSTEM para acceder a la pila del proceso interrumpido.
	asm volatile("cps #0x1f");

	// push de todos los registros a la pila del proceso detenido.
	// r15 = PC.
	// r14 = LR.
	// r13 = SP. No queremos hacer push del stack pointer -> ARM doesn't guarantee what value it will have.
	asm volatile("push	{r0-r12,r14}");

	// Obtencion de la pila
    asm volatile ("MOV %0, SP\n\t" : "=r" (sp_addr) );

    // Return to SVR mode
    asm volatile("cps #0x13");

	// Obtencion del PC del proceso interrumpido. LR del modo IRQ = PC del proceso interrumpido
	asm volatile ("MOV %0, LR\n\t" : "=r" (lr_addr) );

	// Obtencion del PSR del proceso interrumpido (SPSR).
	asm volatile("MRS %0, SPSR\n\t" : "=r" (spsr) );

	// FIN DEL PROLOGO.
	uart_puts("Valor de sp en poseidon: 0x");
    uart_puts(uintToString(sp_addr,HEXADECIMAL));
	uart_puts("\n\r");
	uart_puts("Valor de lr en poseidon: 0x");
    uart_puts(uintToString(lr_addr,HEXADECIMAL));
	uart_puts("\n\r");

	// Bottom 24 bits of the SWI instruction are the SWI number
	swi = *((unsigned int *)(lr_addr - 4)) & 0x00ffffff;

	// Handle syscall
	new_stack = syscall_handler(sp_addr,lr_addr,spsr,swi);
	_force_to_save_new_stack(new_stack);

	// Cambio a modo SYSTEM para acceder a los registros del nuevo proceso
	asm volatile("cps #0x1f");
	// Asignación de la nueva pila
	asm volatile ("MOV SP, %0\n\t" : "=r" (new_stack) );
	// pop de los registros del proceso
	asm volatile("pop	{r0-r12,r14}");
	// RETORNO DE LA RUTINA DE INTERRUPCION. hace un restore del pc y el SPSR slvado por SRSDB
	asm volatile("RFEIA	sp!");

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

	print_kernel_panic();

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

	uart_puts("[ERROR] Data abort! (Falta de pagina): ");
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
void __attribute__((naked)) interrupt_vector(void) {

	unsigned int pc;
	unsigned int stack_pointer;
	unsigned int spsr;
	unsigned int new_stack;

	// COMIENZO DEL PROLOGO DE LA RTI.
	asm volatile("sub lr,lr,#4");

	// PUSH PC y SPSR en la pila del proceso interrumpido.
	asm volatile("SRSDB #16!");

	// Cambio a modo SYSTEM para acceder a la pila del proceso interrumpido.
	asm volatile("cps #0x1f");

	// push de todos los registros a la pila del proceso detenido.
	// r15 = PC.
	// r14 = LR.
	// r13 = SP. No queremos hacer push del stack pointer -> ARM doesn't guarantee what value it will have.
	asm volatile("push	{r0-r12,r14}");

	// Obtencion de la pila
    asm volatile ("MOV %0, SP\n\t" : "=r" (stack_pointer) );

    // Vuelta al modo IRQ para el manejo de la interrupción.
	asm volatile("cps #0x12");

	// Obtencion del PC del proceso interrumpido. LR del modo IRQ = PC del proceso interrumpido
	asm volatile ("MOV %0, LR\n\t" : "=r" (pc) );

	// Obtencion del PSR del proceso interrumpido (SPSR).
	asm volatile("MRS %0, SPSR\n\t" : "=r" (spsr) );

	// FIN DEL PROLOGO.

    // Comprobar si la IRQ ha sido producida por la UART0
    if(rpiIRQController->IRQ_pending_2 != 0) {
    	// La IRQ ha sido producida por la UART0.
        *((unsigned int *) UART0_ICR) = 0x7FF;
        rpiIRQController->IRQ_pending_1 = rpiIRQController->IRQ_pending_2 = 0;

        new_stack = uart_interrupt_handler(stack_pointer,pc);

    } else {
    	// La IRQ Ha sido producida por el timer.
    	// Se invoca al dispatcher para el cambio de contexto

		timer_reset();

		new_stack = schedule_timeout(stack_pointer,pc,spsr);
    }

	_force_to_save_new_stack(new_stack);

    // COMIENZO DEL EPILOGO DE LA RTI.

	// Cambio a modo SYSTEM para acceder a los registros del nuevo proceso
	asm volatile("cps #0x1f");
	// Asignación de la nueva pila
	asm volatile ("MOV SP, %0\n\t" : "=r" (new_stack) );
	// pop de los registros del proceso
	asm volatile("pop	{r0-r12,r14}");
	// RETORNO DE LA RUTINA DE INTERRUPCION. hace un restore del pc y el SPSR slvado por SRSDB
	asm volatile("RFEIA	sp!");

    // FIN DEL EPILOGO DE LA RTI.

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

	print_kernel_panic();

	while(1);
}
