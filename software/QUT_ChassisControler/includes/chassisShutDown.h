/**
 * @file chassisShutDown.h
 * @author Jonn Dillon
 * @date 16/4/2018
 * @brief Defines and outlines all varibles and definitions related to the Shut Down Board
 **/

#ifndef CHASSIS_SHUTDOWN_H
#define CHASSIS_SHUTDOWN_H

#include <stdlib.h>
#include <avr/io.h>

//shutdown circuits
//seg 4 pin 63 dig 1 pinj0	//inertia switch
//seg 5 pin 64 dig 2 pinj1	//left front upright
//seg 6 pin 66 dig 3 pinj3	//brake overtravel
//seg 7 pin 68 dig 4 pinj5	//right front upright
//seg 8 pin 77 dig 5 pina1	//driver E-Stop

//digital inputs
//dig 6 pin 90	//driver switch

//digital outputs
//pin 25	//driver lamp a
//pin 24	//driver lamp b

#define STOP_INERTIA_SWITCH			PINJ&(1<<PINJ0) /**< Instruction for Stop Inertia Switch */
#define STOP_LEFT_FRONT_UPRIGHT		PINJ&(1<<PINJ1) /**< Instruction for Stop Left Front Upright */
#define STOP_BRAKE_OVERTRAVEL		PINJ&(1<<PINJ3) /**< Instruction for Stop Brake Overtravel */
#define STOP_RIGHT_FRONT_UPRIGHT	PINJ&(1<<PINJ5) /**< Instruction for Stop Right Front Upright */
#define STOP_DRIVER_ESTOP			PINA&(1<<PINA1) /**< Instruction for Driver Emergency Stop */

#define SHDN_INERTIA_SWITCH			1 /**< Shutdown Flag for Intertia Switch*/
#define SHDN_LEFT_FRONT_UPRIGHT		2 /**< Shutdown Flag for Left Front Upright */
#define SHDN_BRAKE_OVERTRAVEL		4 /**< Shutdown Flag for Brake Overtravel */
#define SHDN_RIGHT_FRONT_UPRIGHT	8 /**< Shutdown Flag for Right Front Upright */
#define SHDN_DRIVER_ESTOP			16 /**< Shutdown Flag for Driver Emergency Stop */
#define IGNITION_PORT				PINJ /**< Ignition Port Definition */
#define IGNITION_PIN				PJ6 /**< Ignition Pin Definition */

#endif // CHASSIS_SHUTDOWN_H