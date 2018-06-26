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
#define ERROR_PEDALS 			1 /**< OLD ERROR CODE Error with Pedals*/
#define ERROR_BRAKE_PRESSURE 	2 /**< OLD ERROR CODE Error with Brake Pressure*/
#define ERROR_INVERTER_RESPONSE 3 /**< OLD ERROR CODE Error with Inverter*/
#define ERROR_CAN2_RESPONSE		4 /**< OLD ERROR CODE Error with CAN2 Response*/

// Error Codes
#define GEARBOX_TEMPERATURE_LOW 	5		// Gearbox Errors
#define GEARBOX_TEMPERATURE_HIGH 	6
#define RADIATOR_TEMPERATURE_LOW	7		// Radiator Errors
#define RADIATOR_TEMPERATURE_HIGH	8
#define BRAKES_PRESSURE_LOW			9		// Brakes Errors
#define BRAKES_PRESSURE_HIGH		10
#define BRAKES_POSITION_LOW			11
#define BRAKES_POSITION_HIGH		12
#define STEERING_ANGLE_LEFT			13		// Steering Errors (too far left/right)
#define STEERING_ANGLE_RIGHT		14
#define THROTTLE_ERROR				15		// Throttle Errors
#define CAR_UPRIGHT_ERROR			16		// Upright Sensors Error
#define BATTERY_CHARGE				17		// Battery Errors
#define BATTERY_TEMPERATURE_LOW		18
#define BATTERY_TEMPERATURE_HIGH	19

// Error Severity Levels
#define INFO 1 /**< Low Severity Level - Everything is all gravy baby */
#define WARN 2 /**< Moderate Severity Level - This is cause for concern */
#define ERROR 3 /**< Critical Severity Level - Consider shutting the car down */

#define INFOSTRING "INFO" /**< String Representation of Low Severity Level */
#define WARNSTRING "!WARNING!" /**< String Representation of Moderate Severity Level */
#define ERRORSTRING "##ERROR##" /**< String Representation of Critical Severity Level */

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
