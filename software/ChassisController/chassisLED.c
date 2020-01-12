#include "chassisLED.h"

void led_toggle(void)
{
    PORTK ^= 0b00100000;
}

void led_off(void)
{
	PORTK &= ~(0b00100000);
}

void led_on(void)
{
	PORTK |= 0b00100000;
}