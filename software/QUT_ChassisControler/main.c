
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>	
#include "utils/MCP2515.h"
#include "utils/uart.h"
#include "utils/a2d.h"
#include "includes/chassisInit.h"
#include "includes/chassisUART.h"
#include "includes/chassisLED.h"
#include "includes/chassisInput.h"
#include "includes/chassisCAN.h"
#include "includes/chassisError.h"

uint8_t buttonStateDebounceCount = 0; 
bool buttonStateLock = 0;
bool buttonState = 0;

// Variables used in the 1kHz CAN heartbeat loop
#define CAN_HEARTBEAT_TIME_INVERTERS (10)   // Defines the 10ms (100Hz) for the inverter trigger
#define CAN_HEARTBEAT_TIME_DATA (10)        // Defines the 10ms (100Hz) for the data trigger
#define CAN_HEARTBEAT_TIME_POWER (50)       // Defines the 50ms (20Hz) for the power trigger
uint8_t CAN_HEARTBEAT_COUNT_INVERTERS = 0;  // Number of iterations for the inverter heartbeat trigger
uint8_t CAN_HEARTBEAT_COUNT_DATA = 2;       // Number of iterations for the data heartbeat trigger
uint8_t CAN_HEARTBEAT_COUNT_POWER = 4;      // Number of iterations for the power heartbeat trigger

#define CAN_HEARTBEAT_ERROR_DELAY (110)     // Milliseconds without return heartbeat, must be slightly larger than largest heartbeat time x2
uint8_t CAN_HEARTBEAT_ERROR_INVERTERS = 0;  // Time without successfull heartbeat for inverters
uint8_t CAN_HEARTBEAT_ERROR_DATA = 2;       // Time without successfull heartbeat for data
uint8_t CAN_HEARTBEAT_ERROR_POWER = 4;      // Time without successfull heartbeat for power

#define CAN_INPUT_SEND_DELAY (200)          // Defines the 200ms (5Hz) for the input send trigger
uint8_t CAN_INPUT_SEND_TIME = 0;            // Number of iterations for the input send trigger

int main(void) {    

    // Set Up
    firmware_init();
    timer_init();

    // uart_putc('B');
    // uart1_putc('A');
    
    // // Testing pure UART implimentation, not working
    // -----------------
    // char ar[]= "A";
    // // High and low bits
    // UBRR1H = (BUAD_RATE_CALC >> 8); 
    // UBRR1L = BUAD_RATE_CALC; 
    // //////////////// 
    // // transimit and recieve enable
    // UCSR1B = (1 << TXEN1)| (1 << TXCIE1) | (1 << RXEN1) | (1 << RXCIE1); 
    // UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);  //8 bit data format
    // ////////////////////////////////////////////////////////////////
    // int i = 0;
    // while (1){  
    //     for (i = 0; i < strlen(ar); i++){ 
    //         while (( UCSR1A & (1<<UDRE1))  == 0){};
    //         PORTK ^= 0b00100000;
    //         UDR1 = ar[i]; 
    //     }
    // }

    // // Testing near-pure ADC implementation, not working
    // // -----------------
    // ADMUX &= ~(1<<ADLAR); //clear
    // a2dSetPrescaler(ADC_PRESCALE_DIV128);
	// a2dSetReference(ADC_REFERENCE_AVCC);
    // ADCSRA |= (1 << ADEN);
    // a2dSetChannel(5);

    // // Testing pure ADC implementation, not working
    // -----------------
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz

    ADMUX |= (1 << REFS0);  // Set ADC reference to AVCC
    ADMUX &= ~(1 << ADLAR);  // Right adjust ADC result to allow easy 8 bit reading

    ADMUX |= 0b00000101;    // Setting to ADC5

    ADCSRA |= (1 << ADEN);  // Enable ADC
    ADCSRA |= (1 << ADSC);  // Start A2D Conversions

    // Main Poll
    // ------------------------------------------------------------------------
    while(1)
    {

        // // Part of near-pure ADC implementation testing
        if(ADCL > 9) 
        {
            PORTK = 0b00100000;
        } 
        else 
        { 
            PORTK = 0b00000000; 
        }

        // // Part of testing UART, checking to see if any char is in the UART buffers
        // -----------------
        // if(isCharAvailable()) {
        //     PORTK ^= 0b00100000;
        // }
        // if(isCharAvailable_1()) {
        //     PORTK ^= 0b00100000;
        // }

        // // Poll inputs and store into variables
        // uint16_t tmpInputVal;
        // if(INPUT_read_accelPedal(&tmpInputVal) == 0) {
        //     INPUT_accelerationPedal = tmpInputVal;
        // }
        // if(INPUT_get_brakePedal(&tmpInputVal) == 0) {
        //     INPUT_brakePedal = tmpInputVal;
        // }
        // if(INPUT_get_brakePressureBack(&tmpInputVal) == 0) {
        //     INPUT_brakePressureBack = tmpInputVal;
        // }
        // if(INPUT_get_brakePressureFront(&tmpInputVal) == 0) {
        //     INPUT_brakePressureFront = tmpInputVal;
        // }

        // if(INPUT_accelerationPedal < 900) {
        //     PORTK = 0b00100000;
        // } else {
        //     PORTK = 0b00000000;
        // }

        // Getto light blinking
        // if (TCNT0 >= 250)
        // {
        //     // PORTK ^= 0b00100000;
        //     TCNT0 = 0;
        // }
    }
}

/**
 * @brief Core system's timer, 1ms, core of the sudo-RTOS system
 * 
 */
void oneKHzTimer() {

    // PORTK ^= 0b00100000;

    // Check the button state
    // -> 50ms Timer / State Change
    // 50ms debounce, IE hold for 50ms and if held, change state
    // ------------------------------------------------------------------------
    // if(PRESSING_BUTTON) // No idea to which PIN set to check
    // {
    //     // Count up 1ms
    //     buttonStateDebounceCount++;
    //     // If 50ms have been counted
    //     if(buttonStateDebounceCount >= 50) 
    //     {
    //         // Lock the state change till un-press of button
    //         buttonStateLock = true;
    //         // 
    //         if(buttonStateDebounceCount > 254) { buttonStateDebounceCount = 50; }
    //         // Triggering every 50ms
    //         buttonState = !buttonState;
    //         led_toggle();
    //     }
    // } 
    // else 
    // {
    //     buttonStateLock = false;
    //     buttonStateDebounceCount = 0;
    // }

    // Send CAN heartbeats -> Inverters: 100Hz, Data: 100Hz, Power: 20Hz
    // 100Hz = 1 / 100 = 0.01s = 10ms, 20Hz = 1 / 20 = 0.05s = 50ms
    // ------------------------------------------------------------------------
    // if(CAN_HEARTBEAT_COUNT_INVERTERS > CAN_HEARTBEAT_TIME_INVERTERS)
    // {
    //     // Reset inverter heartbeat counter
    //     CAN_HEARTBEAT_COUNT_INVERTERS = 0;
    //     // Send inverter system heartbeat
    //     // CAN_send(MCP2515_CAN1, )
    // }
    // if(CAN_HEARTBEAT_COUNT_DATA > CAN_HEARTBEAT_TIME_DATA)
    // {
    //     // Reset data heartbeat counter
    //     CAN_HEARTBEAT_COUNT_DATA = 0;
    //     // Send data system heartbeat
    //     // CAN_send(MCP2515_CAN2, )
    // }
    // if(CAN_HEARTBEAT_COUNT_POWER > CAN_HEARTBEAT_TIME_POWER)
    // {
    //     // Reset power heartbeat counter
    //     CAN_HEARTBEAT_COUNT_POWER = 0;
    //     // Send power system heartbeat
    //     // CAN_send(MCP2515_CAN2, )
    // }
    // CAN_HEARTBEAT_COUNT_INVERTERS++;
    // CAN_HEARTBEAT_COUNT_DATA++;
    // CAN_HEARTBEAT_COUNT_POWER++;



    // // CAN Error counts -> Missing Receives
    // // ------------------------------------------------------------------------
    // if(CAN_HEARTBEAT_ERROR_INVERTERS > CAN_HEARTBEAT_ERROR_DELAY)
    // {
    //     throw_error_code(ERROR_LEVEL_WARN, ERROR_CANBUS_1_NO_RESPONSE);
    // }
    // if(CAN_HEARTBEAT_ERROR_DATA > CAN_HEARTBEAT_ERROR_DELAY)
    // {
    //     throw_error_code(ERROR_LEVEL_WARN, ERROR_CANBUS_2_NO_RESPONSE);
    // }
    // if(CAN_HEARTBEAT_ERROR_POWER > CAN_HEARTBEAT_ERROR_DELAY)
    // {
    //     throw_error_code(ERROR_LEVEL_WARN, ERROR_CANBUS_3_NO_RESPONSE);
    // }
    // CAN_HEARTBEAT_ERROR_INVERTERS++;
    // CAN_HEARTBEAT_ERROR_DATA++;
    // CAN_HEARTBEAT_ERROR_POWER++;


    // Send CAN input

}

// -------------------------------------------------- Interrupt Service Routines --------------------------------------------------

/**
 * @brief Called whenever the 1Khz timer triggers
 */
ISR(TIMER1_COMPA_vect)
{
    oneKHzTimer();
}

/**
 * @brief Called whenever CANBUS 1 interupt is triggered
 *        * When ever there is data waiting in CAN 1
 */
ISR(INT1_vect) {
	// Details about the message we're attempting to pull from the CAN bus
	uint8_t data[8];
	uint32_t ID;
	uint8_t numBytes;

	// Get the data from the CAN bus and process it
	CAN_pull_packet(MCP2515_CAN1, &numBytes, data, &ID);

    // If the data packet is crap
    // throw_error_code(ERROR_LEVEL_WARN, ERROR_CANBUS_1_RESPONSE_MALFORMED);
}

/**
 * @brief Called whenever CANBUS 2 interupt is triggered
 *        * When ever there is data waiting in CAN 2
 */
ISR(INT0_vect)	{
	// Details about the message we're attempting to pull from the CAN bus
	uint8_t data[8];
	uint32_t ID;
	uint8_t numBytes;

	// Get the data from the CAN bus and process it
	CAN_pull_packet(MCP2515_CAN2, &numBytes, data, &ID);

    // If the data packet is crap
    // throw_error_code(ERROR_LEVEL_WARN, ERROR_CANBUS_2_RESPONSE_MALFORMED);
}

/**
 * @brief Called whenever CANBUS 3 interupt is triggered
 *        * When ever there is data waiting in CAN 3
 */
ISR(PCINT0_vect) {
	// Details about the message we're attempting to pull from the CAN bus
	uint8_t data[8];
	uint32_t ID;
	uint8_t numBytes;

	// Get the data from the CAN bus and process it
	CAN_pull_packet(MCP2515_CAN3, &numBytes, data, &ID);

    // If the data packet is crap
    // throw_error_code(ERROR_LEVEL_WARN, ERROR_CANBUS_3_RESPONSE_MALFORMED);
}