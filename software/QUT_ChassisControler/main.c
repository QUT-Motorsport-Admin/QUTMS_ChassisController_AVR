
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
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
uint8_t CAN_HEARTBEAT_COUNT_DATA = 1;       // Number of iterations for the data heartbeat trigger
uint8_t CAN_HEARTBEAT_COUNT_POWER = 2;      // Number of iterations for the power heartbeat trigger

#define CAN_HEARTBEAT_ERROR_DELAY (110)     // Milliseconds without return heartbeat, must be slightly larger than largest heartbeat time x2
uint8_t CAN_HEARTBEAT_ERROR_INVERTERS = 100;// Time without successfull heartbeat for inverters
uint8_t CAN_HEARTBEAT_ERROR_DATA = 101;     // Time without successfull heartbeat for data
uint8_t CAN_HEARTBEAT_ERROR_POWER = 102;    // Time without successfull heartbeat for power

#define INPUT_TIME_PEDAL_THROTTLE  (10)     // Defines the 10ms (100Hz) for the input send trigger
#define INPUT_TIME_PEDAL_BRAKE  (10)        // Defines the 10ms (100Hz) for the input send trigger
#define INPUT_TIME_TEMP (100)               // Defines the 100ms (10Hz) for the input send trigger
uint8_t INPUT_PEDAL_THROTTLE_COUNT = 3;     // Number of iterations for the pot heartbeat trigger
uint8_t INPUT_PEDAL_BRAKE_COUNT = 4;        // Number of iterations for the pot heartbeat trigger
uint8_t INPUT_TEMP_COUNT = 5;               // Number of iterations for the temp heartbeat trigger

#define CAN_INPUT_SEND_DELAY (10)           // Defines the 200ms (5Hz) for the input send trigger
uint8_t CAN_INPUT_SEND_TIME = 0;            // Number of iterations for the input send trigger

int main(void) {    

    // Set Up
    firmware_init();
    timer_init();

    // Enable Interupts
    sei();

    // Main Poll
    // ------------------------------------------------------------------------
    while(1)
    {
        for(long i = 0; i < 600000; i++) {

        }
        uart_putc(68);
        uart1_putc(68);
        
    }
}

/**
 * @brief Core system's timer, 1ms, core of the sudo-RTOS system
 */
void oneKHzTimer() {

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



    // CAN Error counts -> Missing Receives
    // ------------------------------------------------------------------------
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
    // uint8_t tmpInputVal;
    if(INPUT_PEDAL_THROTTLE_COUNT > INPUT_TIME_PEDAL_THROTTLE)
    {
        INPUT_accelerationPedal = a2d_10bitCh(5);
        // if(INPUT_get_accelPedal(&tmpInputVal) == 0) {
        //     // PORTK |= 0b00100000;
        //     INPUT_accelerationPedal = tmpInputVal;
        // }
        INPUT_PEDAL_THROTTLE_COUNT = 0;
    }

    // if(INPUT_PEDAL_BRAKE_COUNT > INPUT_TIME_PEDAL_BRAKE)
    // {
    //     if(INPUT_get_brakePedal(&tmpInputVal) == 0) {
    //         INPUT_brakePedal = tmpInputVal;
    //     }
    //     INPUT_PEDAL_BRAKE_COUNT = 0;
    // }
    INPUT_PEDAL_THROTTLE_COUNT++;
    // INPUT_PEDAL_BRAKE_COUNT++;
    

    // if(INPUT_get_brakePressureBack(&tmpInputVal) == 0) {
    //     INPUT_brakePressureBack = tmpInputVal;
    // }
    // if(INPUT_get_brakePressureFront(&tmpInputVal) == 0) {
    //     INPUT_brakePressureFront = tmpInputVal;
    // }
}

// -------------------------------------------------- Interrupt Service Routines --------------------------------------------------

/**
 * @brief Called whenever the 1Khz timer triggers
 */
ISR(TIMER1_COMPA_vect)
{
    oneKHzTimer();
    // led_toggle();
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