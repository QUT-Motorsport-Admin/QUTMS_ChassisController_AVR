/**
 * @file main.h
 * @author Jonn Dillon
 * @date file creation 16/4/2018
 * @description This file defines all functions and operations related to the Power Distribution Unit and Shutdown board
 **/


#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef MAIN_H
#define MAIN_H

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

unsigned char enableSwitch_A = 0;

unsigned int MC_oilTemp[4] = {0,0,0,0};

unsigned int AN1_voltage = 0;
unsigned int AN2_voltage = 0;

unsigned char operatingMode = 0;

unsigned int T1_temp = 0;

#define INVERTERS_ALL 		0b1111
#define TORQUE_COMMAND 		3
#define NORMAL 				1
#define ACCUMULATOR_FRONT 	1
#define ACCUMULATOR_REAR 	2

#endif // MAIN_H