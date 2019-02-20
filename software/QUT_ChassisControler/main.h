
#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// EEPROM Values that need to be editable later, (root)
#define CORE_INVERTER_FRONT_LEFT    (0b00000001)
#define CORE_INVERTER_FRONT_RIGHT   (0b00000010)
#define CORE_INVERTER_BACK_RIGHT    (0b00000100)
#define CORE_INVERTER_BACK_LEFT     (0b00001000)
extern uint8_t CORE_invertersPresent;

// Variables used in the 1kHz CAN heartbeat loop
#define CAN_HEARTBEAT_TIME_INVERTERS (10)   // Defines the 10ms (100Hz) for the inverter trigger
#define CAN_HEARTBEAT_TIME_DATA (10)        // Defines the 10ms (100Hz) for the data trigger
#define CAN_HEARTBEAT_TIME_POWER (50)       // Defines the 50ms (20Hz) for the power trigger

#define CAN_HEARTBEAT_ERROR_DELAY (110)     // Milliseconds without return heartbeat, must be slightly larger than largest heartbeat time x2

#define INPUT_TIME_PEDAL_THROTTLE  (10)     // Defines the 10ms (100Hz) for the input send trigger
#define INPUT_TIME_PEDAL_BRAKE  (10)        // Defines the 10ms (100Hz) for the input send trigger
#define INPUT_TIME_TEMP (100)               // Defines the 100ms (10Hz) for the input send trigger

#define CAN_INPUT_SEND_DELAY (10)           // Defines the 200ms (5Hz) for the input send trigger

// Variables for the car's various states
#define CAR_ARM_HV (1)                      // Used to set the car's armed state to high
#define CAR_DISARM_HV (0)                   // Used to set the car's armed state to low
extern uint8_t armedState;                  // Stores the car's armed state
extern uint8_t ignitionState;               // Stores the car's ignition switch/button state

extern uint8_t shutdownState;               // Stores the car's current state of its shutdown circuity

#endif // MAIN_H