/**
 * @file chassisInput.h
 * @author Jonn Dillon
 * @date 16/4/2018
 * @brief Defines and outlines all variables and definitions related to everything related to input within the car.
 **/

#ifndef CHASSIS_INPUT_H
#define CHASSIS_INPUT_H

#include <stdlib.h>
#include <avr/io.h>

#define PEDAL_BRAKE_HIGH		900	/**< Temporary Value, High Brake Pedal Application */
#define PEDAL_BRAKE_LOW			100	/**< Temporary Value, Low Brake Pedal Application */
#define PEDAL_BRAKE_LIGHT_ON 	512 /**< Temporary Value, must be updated with testing. Moderate Brake Pedal Application */

#define PEDAL_THROTTLE_HIGH		900	/**< High Throttle Pedal Application */
#define PEDAL_THROTTLE_LOW		100	/**< Tempoary Value, Low Throttle Pedal Application */
#define PEDAL_DELTA_THRESH_L	50 /**< Low Value for pedal sensor discrepancy */
#define PEDAL_DELTA_THRESH_H	150 /**< High Value for pedal sensor discrepancy */

/**
 * @brief Data Structure for Steering Wheel flags
 * 
 */
typedef struct STEERINGWHEEL
{
	uint8_t flags[4];

} STEERINGWHEEL;

/**
 * @brief Storage for Steering Wheel flags
 * 
 */
struct STEERINGWHEEL steeringWheel;

/**
 * @brief Steering Wheel Angle
 * 
 */
uint16_t	 steeringAngle = 0;

#endif // CHASSIS_INPUT_H