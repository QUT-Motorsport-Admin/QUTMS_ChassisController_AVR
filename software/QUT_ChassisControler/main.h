
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
#include "includes/chassisInput.h"
#include "includes/chassisUART.h"
#include "includes/chassisLED.h"
#include "includes/chassisCAN.h"
#include "includes/chassisError.h"

// Variables used in the 1kHz CAN heartbeat loop
#define CAN_HEARTBEAT_TIME_INVERTERS (10)   // Defines the 10ms (100Hz) for the inverter trigger
#define CAN_HEARTBEAT_TIME_DATA (10)        // Defines the 10ms (100Hz) for the data trigger
#define CAN_HEARTBEAT_TIME_POWER (50)       // Defines the 50ms (20Hz) for the power trigger
uint8_t CanHeartbeatCountInverters = 0;     // Number of iterations for the inverter heartbeat trigger
uint8_t CanHeartbeatCountData = 1;          // Number of iterations for the data heartbeat trigger
uint8_t CanHeartbeatCountPower = 2;         // Number of iterations for the power heartbeat trigger

#define CAN_HEARTBEAT_ERROR_DELAY (110)     // Milliseconds without return heartbeat, must be slightly larger than largest heartbeat time x2
uint8_t CanHeartbeatErrorInverters = 100;   // Time without successfull heartbeat for inverters
uint8_t CanHeartbeatErrorData = 101;        // Time without successfull heartbeat for data
uint8_t CanHeartbeatErrorPower = 102;       // Time without successfull heartbeat for power

#define INPUT_TIME_PEDAL_THROTTLE  (10)     // Defines the 10ms (100Hz) for the input send trigger
#define INPUT_TIME_PEDAL_BRAKE  (10)        // Defines the 10ms (100Hz) for the input send trigger
#define INPUT_TIME_TEMP (100)               // Defines the 100ms (10Hz) for the input send trigger
uint8_t InputPedalThrottleCount = 3;        // Number of iterations for the pot heartbeat trigger
uint8_t InputPedalBrakeCount = 4;           // Number of iterations for the pot heartbeat trigger
uint8_t InputTempCount = 5;                 // Number of iterations for the temp heartbeat trigger

#define CAN_INPUT_SEND_DELAY (10)           // Defines the 200ms (5Hz) for the input send trigger
uint8_t CANInputSendTime = 0;               // Number of iterations for the input send trigger