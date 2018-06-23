/**
 * @file chassisShutDown.h
 * @author Jonn Dillon
 * @date 16/4/2018
 * @description Handles functions, calls and utilities related to the Error system
 **/

#include "chassisError.h";

/**
 * throw_error_code()
 * Input:	severity	- Severity of the error (INFO, WARN, or ERROR)
 * 			error_code		- Error to report
 * Returns: none
 * 
 * Not implemented yet
 **/
void throw_error_code(uint16_t severity, uint16_t error_code)
{	
	switch (error_code) {
		// Example - Error Codes TBD
		case ERROR_BRAKE_PRESSURE:					
			switch (severity) {
				case INFO:
				// Logging behaviour for Info-level logs
				send_error_message(INFOSTRING, "Brake Pressue Normal")
				break;

				case WARN;
				// Loggging behaviour for Warning-level logs
				send_error_message(WARNSTRING, "Break pressure abnormal!");
				break;

				case ERROR:
				// Loggign behaviour for Error-level logs
				send_error_message(ERRORSTRING, "BREAK PRESSURE CRITICAL");
				break;
			}			
		break;
		
		// case ERROR_CODE:					
		// 	switch (severity) {
		// 		case INFO:
		// 		// Logging behaviour for Info-level logs
		// 		send_error_message(INFOSTRING, "Info Description")
		//		// Additional Info Handling
		// 		break;

		// 		case WARN;
		// 		// Loggging behaviour for Warning-level logs
		// 		send_error_message(WARNSTRING, "Warning Description!");
		//		// Additional Warning Handling
		// 		break;

		// 		case ERROR:
		// 		// Loggign behaviour for Error-level logs
		// 		send_error_message(ERRORSTRING, "ERROR DESCRIPTION");
		//		// Additional Error Handling
		// 		break;
		// 	}			
		// break;

	}		
}

/**
 * send_error_message()
 * Input: 	start	- String representation of log level
 * 			message	- Message to send - should not exceed 40 characters
 * Returns: none
 * 
 * Formats a 50-character long log message, and sends it over UART.
 * */
void send_error_message(char start[], char message[]) 
{
	unsigned char *errorData;
	snprintf(errorData, 50, "%s %s", start, message);
	uart_send_data(errorData, 50);
}

/**
 * shutdown_probe()
 * Input:	none
 * Returns: none
 * 
 * Checks if a shutdown switch has been activated. Sends the method of shutdown to be recorded
 * 
 * Reference: ATmega Datasheet Chapter 13 (I/O-Ports)
 **/
void shutdown_probe()
{
	if(STOP_BRAKE_OVERTRAVEL)shutdown_state(SHDN_BRAKE_OVERTRAVEL);
	if(STOP_DRIVER_ESTOP)shutdown_state(SHDN_DRIVER_ESTOP);
	if(STOP_INERTIA_SWITCH)shutdown_state(SHDN_INERTIA_SWITCH);
	if(STOP_LEFT_FRONT_UPRIGHT)shutdown_state(SHDN_LEFT_FRONT_UPRIGHT);
	if(STOP_RIGHT_FRONT_UPRIGHT)shutdown_state(SHDN_RIGHT_FRONT_UPRIGHT);
}

/**
 * shutdown_state()
 * Input:	shutdownFlag	-
 * Returns: none
 * 
 * Not implemented yet
 **/
void shutdown_state(uint16_t shutdownFlag)
{
	//do shutdown reporting here
}