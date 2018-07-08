/**
 * @brief Defines and outlines all varibles and definitions related to the error system.
 * 
 * @file chassisError.h
 * @author Jonn Dillon
 * @date 2018-04-16
 */

#ifndef CHASSIS_ERROR_H
#define CHASSIS_ERROR_H

#include <stdlib.h>
#include <avr/io.h>

#define MAX_ERROR_MESSAGE_LENGTH 40 /**< Error messages (excluding the severity level string) should not exceed this length */

// Old
// #define ERROR_PEDALS 			1 /**< OLD ERROR CODE Error with Pedals*/
// #define ERROR_BRAKE_PRESSURE 	2 /**< OLD ERROR CODE Error with Brake Pressure*/
// #define ERROR_INVERTER_RESPONSE  3 /**< OLD ERROR CODE Error with Inverter*/
// #define ERROR_CAN2_RESPONSE		4 /**< OLD ERROR CODE Error with CAN2 Response*/

// Error Codes
#define ERROR_GENERAL                   (1)     /**< Error Code: Generic error for the system */

#define ERROR_CANBUS_1                  (2)     /**< Error Code: */
#define ERROR_CANBUS_1                  (3)     /**< Error Code: */
#define ERROR_CANBUS_1                  (4)     /**< Error Code: */

#define ERROR_GEARBOX_TEMPERATURE_LOW 	(5)     /**< Error Code: Gearbox X Temperature Too Low */
#define ERROR_GEARBOX_TEMPERATURE_HIGH 	(6)     /**< Error Code: Gearbox X Temperature Too High*/
#define ERROR_RADIATOR_TEMPERATURE_LOW	(7)     /**< Error Code: Radiator X Temperature Too Low */
#define ERROR_RADIATOR_TEMPERATURE_HIGH	(8)     /**< Error Code: Radiator X Temperature Too High */
#define ERROR_BRAKES_PRESSURE_LOW		(9)     /**< Error Code: Brake X Pressure Too Low */
#define ERROR_BRAKES_PRESSURE_HIGH		(10)    /**< Error Code: Brake X Pressure Too High */
#define ERROR_BRAKES_POSITION_LOW		(11)    /**< Error Code: Brake X Position Too Low */
#define ERROR_BRAKES_POSITION_HIGH		(12)    /**< Error Code: Brake X Position Too High */
#define ERROR_STEERING_ANGLE_LEFT		(13)    /**< Error Code: Steering Wheel Turned Too Far Left */
#define ERROR_STEERING_ANGLE_RIGHT		(14)    /**< Error Code: Steering Wheel Turned Too Far Right */
#define ERROR_THROTTLE_ERROR			(15)    /**< Error Code: Problem With Throttle */
#define ERROR_CAR_UPRIGHT_ERROR			(16)    /**< Error Code: Car No Longer Upgright (Has Rolled) */
#define ERROR_BATTERY_CHARGE			(17)    /**< Error Code: Problem with Battery Charge */
#define ERROR_BATTERY_TEMPERATURE_LOW	(18)    /**< Error Code: Battery Temperature Too Low */
#define ERROR_BATTERY_TEMPERATURE_HIGH	(19)    /**< Error Code: Battery Temperature Too High */

// Error Severity Levels
#define ERROR_LEVEL_INFO    (1)         /**< Low Severity Level - Everything is all gravy baby */
#define ERROR_LEVEL_WARN    (2)         /**< Moderate Severity Level - This is cause for concern */
#define ERROR_LEVEL_ERROR   (3)         /**< Critical Severity Level - Consider shutting the car down */

#define ERROR_STRING_INFO   "INFO"      /**< String Representation of Low Severity Level */
#define ERROR_STRING_WARN   "!WARNING!" /**< String Representation of Moderate Severity Level */
#define ERROR_STRING_ERROR  "##ERROR##" /**< String Representation of Critical Severity Level */

/**
 * @brief Generates an error message, and takes the necessary actions depending on the severity and error. 
 * 
 * Uses 0 as the part number.
 * 
 * Example Code: 
 * 
 * throw_error_code(WARN, STEERING_ANGLE_LEFT);
 * 
 * @author Shane Havers 
 * 
 * @param severity Severity of the error (INFO, WARN, or ERROR)
 * @param error_code Error to report 
 * 
 * @return void
 */
void throw_error_code(uint16_t severity, uint16_t error_code);

/**
 * @brief Generatres an error message, and takes the necessary actions depending on the severity and error.
 * 
 * Example Code:
 * 
 * throw_error_code_with_number(ERROR, GEARBOX_TEMPERATURE_HIGH, 3);
 * 
 * @author Shane Havers
 * 
 * @param severity Severity of the error (INFO, WARN, or ERROR)
 * @param error_code Error to report
 * @param part_number The number of the part generating the error (i.e. Gearbox 1, Radiator 2 etc.)
 * 
 * @return void
 * 
 */
void throw_error_code_with_number(uint16_t severity, uint16_t error_code, uint16_t part_number);

/**
 * @brief Formats a 50-character long log message, and sends it over UART
 * 
 * This method is called by throw_error_code() and throw_error_code_with_number().
 * There shouldn't be any need to call it anywhere else. 
 * 
 * @author Shane Havers
 * 
 * @param start String representation of log level
 * @param message Message to send - should not exceed 40 characters
 * @param part_number The number of the part generating the error (i.e. Gearbox 1, Radiator 2 etc.)
 * 
 * @return void
 */
void send_error_message(char start[], char message[], uint16_t part_number);

#endif // CHASSIS_ERROR_H
