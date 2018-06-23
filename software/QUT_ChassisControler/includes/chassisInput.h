/**
 * @file chassisShutDown
 * @author Jonn Dillon
 * @date 16/4/2018
 * @description Defines and outlines all variables and definitions related to everything related to input within the car.
 **/

#ifndef CHASSIS_INPUT_H
#define CHASSIS_INPUT_H

#include <stdlib.h>
#include <avr/io.h>

#define PEDAL_BRAKE_HIGH		900	//temp value
#define PEDAL_BRAKE_LOW			100	//temp value
#define PEDAL_BRAKE_LIGHT_ON 	512 //temp value, must update with testing

#define PEDAL_THROTTLE_HIGH		900	//temp value
#define PEDAL_THROTTLE_LOW		100	//temp value
#define PEDAL_DELTA_THRESH_L	50
#define PEDAL_DELTA_THRESH_H	150

typedef struct STEERINGWHEEL
{
	uint8_t flags[4];

} STEERINGWHEEL;

struct STEERINGWHEEL steeringWheel;
uint16_t	 steeringAngle = 0;

#endif // CHASSIS_INPUT_H