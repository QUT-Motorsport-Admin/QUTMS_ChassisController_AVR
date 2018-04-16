/**
 * @file main.h
 * @author Jonn Dillon
 * @date file creation 16/4/2018
 * @description This file defines all functions and operations related to the Power Distribution Unit and Shutdown board
 **/


#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "MCP2515.h"
#include <stdlib.h>

#define NUM_INVERTERS		4
#define NUM_ACCUMULATORS	1
#define NUM_CMU_PER_AMU		6
#define CMU_CELL_COUNT		8


int testValue1;
int testValue2;

unsigned int temps[0];
unsigned int voltages[8];

unsigned char enableSwitch_A = 0;

unsigned int MC_oilTemp[4] = {0,0,0,0};



unsigned int AN1_voltage = 0;
unsigned int AN2_voltage = 0;
unsigned int currentTorqueDemand[4] = {0, 0, 0, 0};
unsigned char inverterStatus = 0;
unsigned int currentRPM = 0;
unsigned int inverterTemp = 0;
unsigned int inverterDCcurrent = 0;
uint16_t	 steeringAngle = 0;
unsigned char operatingMode = 0;

unsigned int T1_temp = 0;

unsigned int testTimer = 0;
unsigned char CANreceiver = 0;
unsigned int radiator_cals_acewell_22k[27] = {801,	800, 799, 797, 791, 785, 767, 750, 734, 707, 689, 671, 637, 598, 581, 562, 529,
493, 464, 443, 359, 338, 317, 297, 278, 234, 204};
volatile int heartbeatTimer = 0;
char CANdiagnostics[10][20];
uint8_t tempBuffer[10];

#define CANselect PORTC &= ~1
#define CANdeselect	  PORTC |= 1


#define INVERTERS 			1
#define PDM_H 				2
#define AMU_H				3
#define WHEEL 				4
#define INVERTERS_ALL 		0b1111
#define TORQUE_COMMAND 		3
#define NORMAL 				1
#define ACCUMULATOR_FRONT 	1
#define ACCUMULATOR_REAR 	2