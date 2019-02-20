#include "main.h"

uint16_t ignitionStateDebounceCount = 0; 
uint8_t ignitionStateLock = 0;
uint8_t ignitionState = 0;

uint8_t armedState = 0;

uint8_t shutdownState = 0;

uint8_t CORE_invertersPresent = 0b00000000;

int main(void) {    

    // Set Up
    firmware_init();
    timer_init();

    // Grab the state of the shutdown circuity
    shutdownState = 0;

    // Enable Interupts
    sei();

    // Main Poll
    // ------------------------------------------------------------------------
    while(1) { }
}

/**
 * @brief Core system's timer, 1ms, core of the sudo-RTOS system
 */
void oneKHzTimer() {

    // static uint8_t CanHeartbeatCountInverters = 0;     // Number of iterations for the inverter heartbeat trigger
    // static uint8_t CanHeartbeatCountData = 1;          // Number of iterations for the data heartbeat trigger
    // static uint8_t CanHeartbeatCountPower = 2;         // Number of iterations for the power heartbeat trigger

    // static uint8_t CanHeartbeatErrorInverters = 100;   // Time without successfull heartbeat for inverters
    // static uint8_t CanHeartbeatErrorData = 101;        // Time without successfull heartbeat for data
    // static uint8_t CanHeartbeatErrorPower = 102;       // Time without successfull heartbeat for power

    static uint8_t InputPedalThrottleCount = 3;        // Number of iterations for the pot heartbeat trigger
    static uint8_t InputPedalBrakeCount = 4;           // Number of iterations for the pot heartbeat trigger
    // static uint8_t InputTempCount = 5;                 // Number of iterations for the temp heartbeat trigger

    // static uint8_t CANInputSendTime = 0;               // Number of iterations for the input send trigger



    // Check the ignition button state
    // 1s debounce, IE hold for 50ms and if held, change state
    // ------------------------------------------------------------------------
    if(!(PINJ & (1<<PJ6))) // Checking Pin J6 (69)
    {
        ignitionState = 1; // Tracks that the ignition button is on
        ignitionStateDebounceCount++; // Count up 1ms
        if(ignitionStateDebounceCount >= 1000) // If 1s has been counted
        {
            // Limit the range it exists in
            if(ignitionStateDebounceCount > 5000) { ignitionStateDebounceCount = 1001; }
            // If this is the first time though from a previous press
            if(ignitionStateLock == 0) {
                ignitionStateLock = 1; // Disabled first run though after press
                armedState = !armedState; // Flips armed state
            }
        }
    } 
    else 
    {
        ignitionState = 0; // Tracks that the ignition button is off
        ignitionStateLock = 0; // Re-enables the first run though after the timer has been reached
        ignitionStateDebounceCount = 0; // Resets the counter for time the button is pressed
    }



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
        INPUT_accelerationPedal = a2d_10bitCh(INPUT_PEDAL_THROTTLE_CH1);
        // if(INPUT_get_accelPedal(&tmpInputVal) == 0) {
        //     // PORTK |= 0b00100000;
        //     INPUT_accelerationPedal = tmpInputVal;
        // }
        InputPedalThrottleCount = 0;
    }

    if(InputPedalBrakeCount > INPUT_TIME_PEDAL_BRAKE)
    {
        INPUT_brakePedal = a2d_10bitCh(INPUT_PEDAL_BRAKE_CH1);
        // if(INPUT_get_brakePedal(&tmpInputVal) == 0) {
        //     INPUT_brakePedal = tmpInputVal;
        // }
        InputPedalBrakeCount = 0;
    }
    InputPedalThrottleCount++;
    InputPedalBrakeCount++;
    

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