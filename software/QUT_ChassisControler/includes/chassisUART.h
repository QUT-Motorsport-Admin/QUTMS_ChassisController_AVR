/**
 * @file chassisUART.h
 * @author Jonn Dillon
 * @date file creation 12/4/2018
 * @brief Defines and outlines all variables and definitions related to the UART communications process
 **/

#ifndef CHASSIS_UART_H
#define CHASSIS_UART_H

#include <stdlib.h>
#include <avr/io.h>
#include "includes/chassisInput.h"
#include "utils/uart.h"

void uart_process_byte(char data);
void uart_parse_input(unsigned char* s);
void uart_parse_poke(unsigned char* s);
void uart_send_real_time_data (void);

#endif // CHASSIS_UART_H