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

int main(uint32_t r0, uint32_t r1, uint32_t atagsAddr) {

	/* Primero seteamos a cero toda la bss */

    extern uint32_t * _bss_start;
    extern uint32_t * _bss_end;

    uint32_t * bss = _bss_start;
    while( bss < _bss_end )
        *bss++ = 0;

    /* end of bss setting to 0 */

    habilitar_GPIO_ACT_LED_output();
    apaga_ACT_LED();
	//bgInit(atagsAddr);

    /* Enable the timer interrupt IRQ */
    RPI_GetIrqController()->Enable_Basic_IRQs = RPI_BASIC_ARM_TIMER_IRQ;

    /* Setup the system timer interrupt */
    /* Timer frequency = Clk/256 * 0x400 */
    RPI_GetArmTimer()->Load = 0x800;

    /* Setup the ARM Timer */
    RPI_GetArmTimer()->Control =
            RPI_ARMTIMER_CTRL_23BIT |
            RPI_ARMTIMER_CTRL_ENABLE |
            RPI_ARMTIMER_CTRL_INT_ENABLE |
            RPI_ARMTIMER_CTRL_PRESCALE_256;


    /*  init scheduler */
    init_scheduler();
    /* Enable interrupts! */
    _enable_interrupts();

	while(1) {
		//bgRefresh();
		//RPI_esperarMicroSeconds(500000);
		//bgRefresh();
		//RPI_esperarMicroSeconds(500000);
	}

	return 0;
}
