/*
 * led_flash.cpp
 *
 * Created: 7/21/2018 12:56:53 PM
 * Author : Ant
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void led_toggle(void);

int main(void)
{
    DDRK |= 32;
	TCCR1A = 0;
	TCCR1B = 13;
	OCR1A = 15000;
	TIMSK1 = 2;
	sei();
	
    while (1) 
    {
		//PORTK ^= 32;
		//_delay_ms(500);
    }
}

ISR(TIMER1_COMPA_vect)
{
	led_toggle();
}

void led_toggle(void)
{
	PORTK ^= 32;
}

