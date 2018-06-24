/**
 * @file chassisInverter.h
 * @author Jonn Dillon
 * @date 16/4/2018
 * @brief Defines and outlines all varibles and definitions related to the Inverter/Motor Controller board
 **/

#ifndef CHASSIS_INVERTER_H
#define CHASSIS_INVERTER_H

#include <stdlib.h>
#include <avr/io.h>

typedef struct Inverter
{
	uint8_t ID;
	uint16_t RPM;
	uint16_t temperature;
	uint16_t current;
	uint16_t duty;
}Inverter;

struct Inverter inverters[NUM_INVERTERS];

unsigned char inverterStatus = 0;
unsigned int currentRPM = 0;
unsigned int inverterTemp = 0;
unsigned int inverterDCcurrent = 0;

#endif // CHASSIS_INVERTER_H