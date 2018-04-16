/**
 * @file chassisUART
 * @author Jonn Dillon
 * @date file creation 12/4/2018
 * @description Defines and outlines all varibles and definitions related to the UART communications process
 **/

#include <stdlib.h>
#include <avr/io.h>
#include <../main.h>

/**
 * @function        uart_process_stdin
 * @description     Will mashal the incoming byte stream into useable numbers
 * @author          Jonn Dillon
 * @date_created    12/8/18
 * @date_modified   N/A
 * @param   char    byte            - Singular char to be added to the current stored 
 * @param   uint_t  &completedInt   - The buffer to be filled with the full int if sucessful
 * @returns uint_t                  - Returns 1 if the &completedInt was filled, 0 otherwise
 **/
int uart_process_stdin(char byte, int &completedInt);


/**
 * @function        uart_send_data
 * @description     Will take a char array and send it to the connected computer
 *                  This function dose no modifications to the string
 * @author          Jonn Dillon
 * @date_created    13/8/18
 * @date_modified   N/A
 * @param   unsigned char * - The given char array to send back to the machine
 * @param   int             - The length of the char array
 * @returns void            - Returns 1 when sucessful/completed
 **/
int uart_send_data(unsigned char* s, int length);