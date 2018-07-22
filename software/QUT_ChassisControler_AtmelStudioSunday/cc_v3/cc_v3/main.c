/*
 * cc_v3.c
 *
 * Created: 7/22/2018 2:43:24 PM
 * Author : Ant
 */ 
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"

void oneKHzTimer(void);
volatile uint8_t tempArm = 0;

int main(void)
{
	uart_init(19200);
    uart1_init(19200);
	DDRD |= 8;				//PD3 = UART1_TX
	DDRE |= 2;				//PE1 = UART0_TX
	DDRK |= 32;				//PK5 = LED
	
	PORTK = 32;				//LED on
	PORTJ |= 64;
	
	TCCR0A |= (1 << WGM01);                 // Setting CTC on timer0
	TCCR0B |= (1 << CS01)|(1 << CS00);      // Set up 64 prescaler (16Mhz / 64)
	OCR0A = 250;                            // (16*10^6) / (1000 * 64) assuming 16Mhz chip = 1Khz
	TIMSK0 |= (1 << OCIE0A);                // Enable COMPA interupt
	
	sei();
	
	uart_putc(68);			//send a "D"
	_delay_ms(250);
    while (1) 
    {
		//_delay_ms(100);
		//uart_putc(68);
		if(tempArm > 0) PORTK |= 32;
		else PORTK &= ~32;
    }
}

ISR(TIMER0_COMPA_vect)
{
	oneKHzTimer();
}

void oneKHzTimer (void)
{
	if((PINJ & 64) == 0)tempArm = 1;
}
