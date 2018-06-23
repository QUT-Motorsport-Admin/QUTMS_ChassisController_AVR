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
