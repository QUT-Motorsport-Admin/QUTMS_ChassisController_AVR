/**
 * @file chassisUART.h
 * @author Jonn Dillon
 * @date file creation 12/4/2018
 * @brief Defines and outlines all varibles and definitions related to the UART communications process
 **/

#ifndef CHASSIS_UART_H
#define CHASSIS_UART_H

#include <stdlib.h>
#include <avr/io.h>
#include "utils/uart.h"

/**
 * @brief UART_getString
 *
 * Will return a completed string from the UART
 * This should called everytime there is an an avaliable byte in the stdIN buffer
 * 
 * @author Jonn Dillon
 */
char *UART_getString(void);


/**
 * @brief UART_sendString
 * 
 * Will take a string pointer, and send it down UART
 * One way method, no error reporting
 *
 * @param s Given string pointer to send (char[])
 */
// void UART_sendString(unsigned char *s);

#endif // CHASSIS_UART_H