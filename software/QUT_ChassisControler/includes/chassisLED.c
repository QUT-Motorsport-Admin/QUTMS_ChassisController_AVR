/**
 * @file chassisLED.c
 * @author Jonn Dillon
 * @date 16/4/2018
 * @brief Handles all operations related to the onboard LED
 **/

#include "chassisLED.h"

/**
 * led_flash()
 * Input:	times - The amount of times to make the LED blink
 * Returns: none
 * 
 * Makes the LED blink a set number of times.
 * The delay between the state of the LED (on/off) is 50ms. The period of the blinks is 100ms
 * 
 * Reference: ATmega Datasheet Chapter 13 (I/O-Ports)
 **/
void led_flash(unsigned char times)
{
	for(uint8_t i = 0; i < times; i++)
	{
		led_toggle();
		_delay_ms(50);
		led_toggle();
		_delay_ms(50);
	}
}

/**
 * led_toggle()
 * Input:	none
 * Returns: none
 * 
 * Switches between the two statues of the LED (on -> off or off -> on)
 * 
 * Reference: ATmega Datasheet Chapter 13 (I/O-Ports)
 **/
void led_toggle()
{
	PORTK ^= 1<<PINK5;
}

/**
 * led_off()
 * Input:	none
 * Returns: none
 * 
 * Turns off the LED
 * 
 * Reference: ATmega Datasheet Chapter 13 (I/O-Ports)
 **/
void led_off()
{
	PORTK &= ~(1<<PINK5);
}