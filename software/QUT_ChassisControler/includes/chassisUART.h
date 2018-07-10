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

/**
 * @brief Will mashal the incoming byte stream into useable numbers 
 * 
 * This should called everytime there is an an avliable byte in the stdIN buffer
 * 
 * @author Jonn Dillon
 * 
 * @param byte Singular char to be added to the current stored
 * @param completedInt The buffer to be filled with the full int if sucessful
 * @return int Returns 1 if the &completedInt was filled, 0 otherwise
 */
int uart_process_stdin(char byte, uint8_t *completedInt);

/**
 * @brief Will take a char array and send it to the connected computer
 * 
 * This function does no modifications to the string
 * 
 * @author Jonn Dillon
 * 
 * @param s The given char array to send back to the machine
 * @param length The length of the char array
 * @return int Returns 1 when sucessful/completed
 */
int uart_send_data(unsigned char* s, int length);

/**
 * @brief Process a CAN packed received via UART
 * 
 * @param s Pointer to packet to process
 * @param addr Address of address to write to
 * @param data Address of data to write to
 * @return int Returns 1 on completion
 */
int uart_process_packet(unsigned char* s, uint16_t &addr, uint32_t &data);

#endif // CHASSIS_UART_H