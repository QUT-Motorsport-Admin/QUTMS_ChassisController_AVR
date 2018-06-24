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

#define MAX_ERROR_MESSAGE_LENGTH 40

#define ERROR_PEDALS 			1
#define ERROR_BRAKE_PRESSURE 	2
#define ERROR_INVERTER_RESPONSE 3
#define ERROR_CAN2_RESPONSE		4

/* Gearbox Error Codes */
#define GEARBOX_TEMPERATURE_GOOD 5
#define GEARBOX_TEMPERATURE_HIGH 6
#define GEARBOX_TEMPERATURE_LOW 7

/* Radiator Error Codes */
#define RADIATOR_TEMPERATURE_GOOD 17
#define RADIATOR_TEMPERATURE_HIGH 18
#define RADIATOR_TEMPERATURE_LOW 19

// Error Severity Levels
#define INFO 1 // Everything is all gravy baby
#define WARN 2 // Cause for concern
#define ERROR 3 // System critical, consider shutting down

#define INFOSTRING "INFO"
#define WARNSTRING "!WARNING!"
#define ERRORSTRING "##ERROR##"

/**
 * @brief Generates an error message, and takes the necessary actions depending on the severity and error. Uses 0 as the part number
 * @author Shane Havers 
 * 
 * @param severity Severity of the error (INFO, WARN, or ERROR)
 * @param error_code Error to report 
 * 
 * @return void
 */
void throw_error_code(uint16_t severity, uint16_t error_code);

/**
 * @brief Generatres an error message, and takes the necessary actions depending on the severity and error
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
