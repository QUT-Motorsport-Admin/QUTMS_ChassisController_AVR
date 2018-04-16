/**
 * @file chassisShutDown
 * @author Jonn Dillon
 * @date 16/4/2018
 * @description Defines and outlines all varibles and definitions related to the error system.
 **/

#include <stdlib.h>
#include <avr/io.h>

#define ERROR_PEDALS 			1
#define ERROR_BRAKE_PRESSURE 	2
#define ERROR_INVERTER_RESPONSE 3
#define ERROR_CAN2_RESPONSE		4