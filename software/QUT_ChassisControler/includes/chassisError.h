/**
 * @file chassisShutDown
 * @author Jonn Dillon
 * @date 16/4/2018
 * @description Defines and outlines all varibles and definitions related to the error system.
 **/

#ifndef CHASSIS_ERROR_H
#define CHASSIS_ERROR_H

#include <stdlib.h>
#include <avr/io.h>

#define ERROR_PEDALS 			1
#define ERROR_BRAKE_PRESSURE 	2
#define ERROR_INVERTER_RESPONSE 3
#define ERROR_CAN2_RESPONSE		4

/* Gearbox Error Codes */
#define GEARBOX_1_TEMPERATURE_GOOD 5
#define GEARBOX_1_TEMPERATURE_HIGH 6
#define GEARBOX_1_TEMPERATURE_LOW 7

#define GEARBOX_2_TEMPERATURE_GOOD 8
#define GEARBOX_2_TEMPERATURE_HIGH 9
#define GEARBOX_2_TEMPERATURE_LOW 10

#define GEARBOX_3_TEMPERATURE_GOOD 11
#define GEARBOX_3_TEMPERATURE_HIGH 12
#define GEARBOX_3_TEMPERATURE_LOW 13

#define GEARBOX_4_TEMPERATURE_GOOD 14
#define GEARBOX_4_TEMPERATURE_HIGH 15
#define GEARBOX_4_TEMPERATURE_LOW 16

#define RADIATOR_1_TEMPERATURE_GOOD 17
#define RADIATOR_1_TEMPERATURE_HIGH 18
#define RADIATOR_1_TEMPERATURE_LOW 19

#define RADIATOR_2_TEMPERATURE_GOOD 17
#define RADIATOR_2_TEMPERATURE_HIGH 18
#define RADIATOR_2_TEMPERATURE_LOW 19



// Error Severity Levels
#define INFO 1 // Everything is all gravy baby
#define WARN 2 // Cause for concern
#define ERROR 3 // System critical, consider shutting down

#define INFOSTRING "INFO"
#define WARNSTRING "!WARNING!"
#define ERRORSTRING "##ERROR##"

/**
 * @function        send_error_message
 * @description     Formats a 50-character long log message, and sends it over UART
 * @author          Shane Havers
 * @date_created    23/06/2018
 * @date_modified   N/A
 * @param   char[]  start   - String representation of log level
 * @param   char[]  message - Message to send - should not exceed 40 characters
 * @returns void
 * */
void send_error_message(char start[], char message[]);

#endif // CHASSIS_ERROR_H
