

#include "main.h"


uint8_t buttonStateDebounceCount = 0; 
bool buttonStateLock = 0;
bool buttonState = 0;

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
    // if(CanHeartbeatCountInverters > CAN_HEARTBEAT_TIME_INVERTERS)
    // {
    //     // Reset inverter heartbeat counter
    //     CanHeartbeatCountInverters = 0;
    //     // Send inverter system heartbeat
    //     // CAN_send(MCP2515_CAN1, )
    // }
    // if(CanHeartbeatCountData > CAN_HEARTBEAT_TIME_DATA)
    // {
    //     // Reset data heartbeat counter
    //     CanHeartbeatCountData = 0;
    //     // Send data system heartbeat
    //     // CAN_send(MCP2515_CAN2, )
    // }
    // if(CanHeartbeatCountPower > CAN_HEARTBEAT_TIME_POWER)
    // {
    //     // Reset power heartbeat counter
    //     CanHeartbeatCountPower = 0;
    //     // Send power system heartbeat
    //     // CAN_send(MCP2515_CAN2, )
    // }
    // CanHeartbeatCountInverters++;
    // CanHeartbeatCountData++;
    // CanHeartbeatCountPower++;



    // CAN Error counts -> Missing Receives
    // ------------------------------------------------------------------------
    // if(CanHeartbeatErrorInverters > CAN_HEARTBEAT_ERROR_DELAY)
    // {
    //     throw_error_code(ERROR_LEVEL_WARN, ERROR_CANBUS_1_NO_RESPONSE);
    // }
    // if(CanHeartbeatErrorData > CAN_HEARTBEAT_ERROR_DELAY)
    // {
    //     throw_error_code(ERROR_LEVEL_WARN, ERROR_CANBUS_2_NO_RESPONSE);
    // }
    // if(CanHeartbeatErrorPower > CAN_HEARTBEAT_ERROR_DELAY)
    // {
    //     throw_error_code(ERROR_LEVEL_WARN, ERROR_CANBUS_3_NO_RESPONSE);
    // }
    // CanHeartbeatErrorInverters++;
    // CanHeartbeatErrorData++;
    // CanHeartbeatErrorPower++;


    // Send CAN input
    // uint8_t tmpInputVal;
    if(InputPedalThrottleCount > INPUT_TIME_PEDAL_THROTTLE)
    {
        INPUT_accelerationPedal = a2d_10bitCh(5);
        // if(INPUT_get_accelPedal(&tmpInputVal) == 0) {
        //     // PORTK |= 0b00100000;
        //     INPUT_accelerationPedal = tmpInputVal;
        // }
        InputPedalThrottleCount = 0;
    }

    // if(InputPedalBrakeCount > INPUT_TIME_PEDAL_BRAKE)
    // {
    //     if(INPUT_get_brakePedal(&tmpInputVal) == 0) {
    //         INPUT_brakePedal = tmpInputVal;
    //     }
    //     InputPedalBrakeCount = 0;
    // }
    InputPedalThrottleCount++;
    // InputPedalBrakeCount++;
    

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