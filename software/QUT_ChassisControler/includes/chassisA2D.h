/**
 * @brief Defines and outlines all varibles and definitions related to 
 *        the on chip analog to digital conversion, specific to the chassis
 * 
 * @file chassisA2D.h
 * @author Jonn Dillon
 * @date 16/4/2018
 * 
 **/

#ifndef CHASSIS_A2D_H
#define CHASSIS_A2D_H

#include <stdlib.h>
#include <avr/io.h>

//ADC channel allocation
#define PEDAL_BRAKE_CH1			10	/**< Brake Pedal Channel 1 */
#define PEDAL_BRAKE_CH2			8   /**< Brake Pedal Channel 2 */
#define PEDAL_THROTTLE_CH1		5   /**< Throttle Pedal Channel 1*/
#define PEDAL_THROTTLE_CH2		2   /**< Throttle Pedal Channel 2*/
#define RADIATOR_RIGHT			0	/**< AN5 */
#define RADIATOR_LEFT			12	/**< AN10 */
#define PRESSURE_BRAKE_FRONT	11	/**< AN6 */
#define PRESSURE_BRAKE_REAR		7	/**< AN7 */
#define STEERING_ANGLE			4	/**< AN8 */
//single sensor, no error checking

#define PRESSURE_BRAKE_HIGH		1022 /**< High Brake Pressure */
#define PRESSURE_BRAKE_LOW		1 /**< Low Brake Pressure */

#define ADC_SAMPLES				10 /**< Samples required for ADC */

#endif // CHASSIS_A2D_H