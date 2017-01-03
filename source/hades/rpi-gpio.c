#include "../includes/hades/rpi-gpio.h"

static volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;

void habilitar_GPIO_ACT_LED_output() {
	gpio[LED_GPFSEL] |= (1 << LED_GPFBIT);
}

void apaga_ACT_LED(){
	gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);
	return;
}

void enciende_ACT_LED(){
	gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
	return;
}
