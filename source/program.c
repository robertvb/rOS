#include "includes/rpi-gpio.h"
#include "includes/rpi-systimer.h"

int main(uint32_t r0, uint32_t r1, uint32_t atagsAddr) {

    habilitar_GPIO_ACT_LED_output();
	enciende_ACT_LED();
	bgInit(atagsAddr);
	while(1) {
		enciende_ACT_LED();
		bgRefresh();
		RPI_esperarMicroSeconds(500000);
		apaga_ACT_LED();
		bgRefresh();
		RPI_esperarMicroSeconds(500000);
	}


	return 0;
}
