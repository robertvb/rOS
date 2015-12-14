#include "rpi-gpio.h"
#include "rpi-systimer.h"
#include "gpuFrameBuffer.h"
#include "rpi-atags.h"
#include "string.h"

int main(uint32_t r0, uint32_t r1, uint32_t atagsAddr)
{
        habilitar_GPIO_ACT_LED_output();	
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















