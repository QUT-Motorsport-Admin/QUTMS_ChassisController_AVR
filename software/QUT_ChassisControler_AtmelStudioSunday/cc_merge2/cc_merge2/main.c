#define F_CPU 16000000 
#include "main.h"

uint16_t ignitionStateDebounceCount = 0; 
uint8_t ignitionStateLock = 0;
uint8_t ignitionState = 0;

uint8_t isArmedState = 0;
uint8_t prevArmedState = 0;
uint8_t isSirenOn = 0;
static uint16_t sirenOnDuration = 2000; // 2000 milliseconds = 2 seconds
uint16_t sirenOnCount = 0;
static uint16_t preChargeOnDuration = 250; // 2000 milliseconds = 2 seconds
uint16_t preChargeOnCount = 0;
uint8_t isPreChargeOn = 0;
uint8_t isContactorHighOn = 0;

uint8_t isBrakeLightOn = 0;
uint8_t isCoolingPumpLeftOn = 1;
uint8_t isCoolingPumpRightOn = 1;
uint8_t isCoolingFanLeftOn = 1;
uint8_t isCoolingFanRightOn = 1;

uint8_t shutdownState = 0;

volatile uint8_t testError = 0;

char out[96] = {'\0'}; // CAN 1 SUFF
uint8_t inverterArray[8] = {0,0,0,0,0,0,0,0};
uint8_t PDMarray[5] = {0,0,0,0,0};
uint8_t WheelArray[8] = {0,0,0,0,0,0,0,0};

volatile uint8_t ouft = 0;

int main(void) {    

    // Set Up
    firmware_init();
    timer_init();

    // Grab the state of the shutdown circuity
    shutdownState = 0;
	isSirenOn = 0;
	_delay_ms(500);
    // Enable Interupts
    sei();		
    // Main Poll
    // ------------------------------------------------------------------------
    while(1) {
		
		//if(isCharAvailable() == 1)uart_process_byte(receiveChar());
		//uart1_puts("Hello World!\n");
	}
	
	// return 0; // Never return from an embedded program, does bad things if this ever triggers
}

/**
 * @brief Core system's timer, 1ms, core of the pseudo RTOS system
 */
void oneKHzTimer(void)
{
    static uint16_t CANheartbeatCountInverters = 0;			// Number of iterations for the inverter heartbeat trigger
    static uint16_t CANheartbeatCountWheel = 1;				// Number of iterations for the data heartbeat trigger
    static uint16_t CANheartbeatCountPDM = 2;				// Number of iterations for the power heartbeat trigger
	static uint16_t CANheartbeatCountShutdown = 3;			// Number of iterations for the power heartbeat trigger
	static uint16_t CANheartbeatCountAMU = 4;				// Number of iterations for the power heartbeat trigger

    // static uint8_t CanHeartbeatErrorInverters = 100;		// Time without successfull heartbeat for inverters
    // static uint8_t CanHeartbeatErrorData = 101;			// Time without successfull heartbeat for data
    // static uint8_t CanHeartbeatErrorPower = 102;			// Time without successfull heartbeat for power

    static uint8_t InputPedalThrottleCount = 5;				// Number of iterations for the throttle pedal heartbeat trigger
    static uint8_t InputPedalBrakeCount = 6;				// Number of iterations for the break pedel heartbeat trigger
	static uint8_t InputSteeringCount = 7;					// Number of iterations for the steering angle heartbeat trigger	
	
    // Check the ignition button state
    // 1s debounce, IE hold for 50ms and if held, change state
    // ------------------------------------------------------------------------
	if(!(PINJ & (1<<PJ6))) // Checking Pin J6 (69)
    {
        ignitionState = 1; // Tracks that the ignition button is on
        if(ignitionStateDebounceCount++ > 1500) // If 1.5s has been counted
		{
            //If this is the first time though from a previous press
            if(ignitionStateLock == 0) {
                ignitionStateLock = 1;		// Disabled first run though after press
                isArmedState ^= 1;
				// If the armed state has just been turned on, activate the siren
				if(isArmedState == 1) {
					led_toggle();
					isSirenOn = 1;
					isPreChargeOn = 1;
				} else {
					led_toggle();
					// Turnning it all off just in case
					isContactorHighOn = 0;
					isPreChargeOn = 0;
					isSirenOn = 0;
				}
            }
			
        }
    }
    else {
        ignitionState = 0; // Tracks that the ignition button is off
        ignitionStateLock = 0; // Re-enables the first run though after the timer has been reached
        ignitionStateDebounceCount = 0; // Resets the counter for time the button is pressed
    }
	
	// Additionally, see if the siren needs to sound that the HV is on
	if(isSirenOn == 1) { // Checks to see if the siren should be active
		if(sirenOnCount++ > sirenOnDuration) { // Counts up time, and if over allowance
			isSirenOn = 0; // Turn of siren
			sirenOnCount = 0;
		}
	}
	
	// Manage the Precharge time
	if(isPreChargeOn == 1) { // Checks to see if the siren should be active
		if(preChargeOnCount++ > preChargeOnDuration) { // Counts up time, and if over allowance
			isPreChargeOn = 0;
			preChargeOnCount = 0;
			isContactorHighOn = 1;
		}
	}
	
	// Gather all input pedals
	//------------------------------------------------------------------------
	
    uint8_t tmpInputVal;
    if(InputPedalThrottleCount > INPUT_TIME_PEDAL_THROTTLE)
    {
		//INPUT_accelerationPedal = (uint8_t)(a2d_10bitCh(5)); // Raw ADC sample
        if(INPUT_get_accelPedal(&tmpInputVal) == 0) {
             INPUT_accelerationPedal = tmpInputVal;
        }
        InputPedalThrottleCount = 0;
    }
    if(InputPedalBrakeCount > INPUT_TIME_PEDAL_BRAKE)
    {
        //INPUT_brakePedal = (uint8_t)(a2d_10bitCh(8)); // Raw ADC sample
        if(INPUT_get_brakePedal(&tmpInputVal) == 0) {
             INPUT_brakePedal = tmpInputVal;
		}
        InputPedalBrakeCount = 0;
    }
	if(InputSteeringCount > INPUT_TIME_STEERING)
	{
		//INPUT_steeringAngle = (uint16_t)(a2d_8bitCh(4)); // Raw ADC sample
		if(INPUT_get_steeringWheel(&tmpInputVal) == 0) {
			INPUT_steeringAngle = tmpInputVal;
		}
		InputSteeringCount = 0;
	}
	//if(INPUT_get_brakePressureBack(&tmpInputVal) == 0) {
	//    INPUT_brakePressureBack = tmpInputVal;
	//}
	//if(INPUT_get_brakePressureFront(&tmpInputVal) == 0) {
	//    INPUT_brakePressureFront = tmpInputVal;
	//}
	
    InputPedalThrottleCount++;
    InputPedalBrakeCount++;
	InputSteeringCount++;
	
	// Brake light code
	if(INPUT_brakePedal > INPUT_PEDAL_BRAKE_LIGHT_ON) {
		isBrakeLightOn = 1;
	} else {
		isBrakeLightOn = 0;
	}
    
	// Creating CANBUS Packet data for transmission
	//------------------------------------------------------------------------
	
	inverterArray[0] = INPUT_accelerationPedal;
	inverterArray[1] = INPUT_brakePedal;
	inverterArray[2] = INPUT_steeringAngle;
	inverterArray[3] = isArmedState;
	inverterArray[4] = sirenOnCount;
	inverterArray[5] = isContactorHighOn << 7 | isPreChargeOn << 6 | isSirenOn << 5 | isBrakeLightOn << 4 |
	isCoolingFanLeftOn << 3 | isCoolingPumpLeftOn << 2 | isCoolingFanRightOn << 1 | isCoolingPumpRightOn | 0; // General State
	
	WheelArray[0] = INPUT_accelerationPedal;
    WheelArray[1] = INPUT_brakePedal;
	WheelArray[2] = INPUT_steeringAngle;
	WheelArray[3] = isArmedState;
	WheelArray[4] = sirenOnCount;
	WheelArray[5] = isContactorHighOn << 7 | isPreChargeOn << 6 | isSirenOn << 5 | isBrakeLightOn << 4 |
	isCoolingFanLeftOn << 3 | isCoolingPumpLeftOn << 2 | isCoolingFanRightOn << 1 | isCoolingPumpRightOn | 0; // General State

	// Fill PDM Array
	PDMarray[0] = isContactorHighOn << 7 | isPreChargeOn << 6 | isSirenOn << 5 | isBrakeLightOn << 4 | 
				  isCoolingFanLeftOn << 3 | isCoolingPumpLeftOn << 2 | isCoolingFanRightOn << 1 | isCoolingPumpRightOn | 0; // General State
	PDMarray[1] = 100; // 
	PDMarray[2] = 100; // 
	PDMarray[3] = 100; // 
	PDMarray[4] = 100; // 
	
	// CANBUS Sending
	// Send CAN heartbeats -> Inverters: 100Hz, Data: 100Hz, Power: 20Hz
	// 100Hz = 1 / 100 = 0.01s = 10ms, 20Hz = 1 / 20 = 0.05s = 50ms
	//------------------------------------------------------------------------
	
	if(CANheartbeatCountInverters >= CAN_HEARTBEAT_TIME_INVERTERS) {
		// Reset inverter heartbeat counter
		CANheartbeatCountInverters = 0;
		// Send inverter system heartbeat 0b0100100000000000000000000011110
		CAN_send(TRACTIVE_CAN, 8, inverterArray, HEARTBEAT_INV_ID);
		
	}
	if(CANheartbeatCountWheel > CAN_HEARTBEAT_TIME_WHEEL) {
		// Reset data heartbeat counter
		CANheartbeatCountWheel = 0;
		// Send data system heartbeat
		CAN_send(DATA_CAN, 8, WheelArray, HEARTBEAT_WHEEL_ID | 1);
	}
	if(CANheartbeatCountPDM > CAN_HEARTBEAT_TIME_PDM) {
		// Reset power heartbeat counter
		CANheartbeatCountPDM = 0;
		// isContactorHighOn ^= 1;
		// Send power system heartbeat (5 bytes in PDM array)
		CAN_send(POWER_CAN, 5, PDMarray, HEARTBEAT_PDM_ID | 1);
	}
	if(CANheartbeatCountShutdown > CAN_HEARTBEAT_TIME_SHUTDOWN)
	{
		// Reset power heartbeat counter
		CANheartbeatCountShutdown = 0;
		// Send shutdown heartbeat (dont care what for now) (5 bytes in PDM array)
		// CAN_send(POWER_CAN, 5, WheelArray, HEARTBEAT_SHUTDOWN_ID | 1);
	}
	if(CANheartbeatCountAMU > CAN_HEARTBEAT_TIME_AMU) {
		// Reset power heartbeat counter
		CANheartbeatCountAMU = 0;
		// Send shutdown heartbeat (dont care what for now)
		// CAN_send(POWER_CAN, 5, PDMarray, HEARTBEAT_AMU_ID | 1);
		UART_formTestPacket();
	}
	
	// the adding commented to test a counting system in the inverters if statement
	CANheartbeatCountInverters++;
	CANheartbeatCountWheel++;
	CANheartbeatCountPDM++;
	CANheartbeatCountShutdown++;
	CANheartbeatCountAMU++;
	
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
	
}

// -------------------------------------------------- Interrupt Service Routines --------------------------------------------------

/**
 * @brief Called whenever the 1Khz timer triggers
 */
ISR(TIMER0_COMPA_vect)
{
    oneKHzTimer();
}

ISR(TIMER1_COMPA_vect)
{

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
	CAN_pull_packet(TRACTIVE_CAN, &numBytes, data, &ID);
	led_toggle();

    // If the data packet is crap
    // throw_error_code(ERROR_LEVEL_WARN, ERROR_CANBUS_1_RESPONSE_MALFORMED);
	//out++;
	
	//sprintf(out, "%x,%x,%x,%x,%x,%x,%x,%x", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
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
	//led_toggle();
	// Get the data from the CAN bus and process it
	CAN_pull_packet(POWER_CAN, &numBytes, data, &ID);

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
	//led_toggle();
	// Get the data from the CAN bus and process it
	CAN_pull_packet(DATA_CAN, &numBytes, data, &ID);

    // If the data packet is crap
    // throw_error_code(ERROR_LEVEL_WARN, ERROR_CANBUS_3_RESPONSE_MALFORMED);

	

}