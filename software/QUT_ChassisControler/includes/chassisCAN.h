/**
 * @file chassisShutDown
 * @author Jonn Dillon
 * @date 16/4/2018
 * @description Defines and outlines all varibles and definitions related to the CANBUS system
 **/

#ifndef CHASSIS_CAN_H
#define CHASSIS_CAN_H

#include <stdlib.h>
#include <avr/io.h>

#define STATUS_REG 			GPIOR0
#define CAN1_DataWaiting	1
#define CAN2_DataWaiting	2
#define CAN3_DataWaiting	4
#define IGNITION			8

// Destination IDs for send_heartbeat()
#define INVERTERS_ID    0
#define AMU_ID          1
#define PDM_ID          2
#define WHEEL_ID        3

#define CAN_ID_INV	        0x0C000000
#define	CAN_ID_AMU	        0x0A000000
#define CAN_ID_PDM	        0x09000000
#define CAN_ID_WHEEL	    0x00400000

unsigned char CANreceiver = 0;
unsigned int radiator_cals_acewell_22k[27] = {801,	800, 799, 797, 791, 785, 767, 750, 734, 707, 689, 671, 637, 598, 581, 562, 529,
493, 464, 443, 359, 338, 317, 297, 278, 234, 204};
volatile int heartbeatTimer = 0;
char CANdiagnostics[10][20];
uint8_t tempBuffer[10];

#define CANselect PORTC &= ~1
#define CANdeselect	  PORTC |= 1

// Pointer to the function that will process the resulting data
void (*CAN_process_data_ptr)(uint8_t, uint8_t, uint8_t, uint32_t);

// ===================================== FUNCTION DECLARATIONS =====================================

/**
 * @function        CAN_send_heartbeat
 * @description     Called periodically in order to send packets through the CAN bus to the other systems
                    in the car. The packets will contain data with flags set for each system, sensor status,
                    etc. 
 * @author          
 * @date_created    
 * @date_modified   23/06/2018
 * @param   unsigned char destination - 
            unsigned char type        -
            unsigned char address     -
 * @returns void
 **/
void CAN_send_heartbeat(unsigned char destination, unsigned char type, unsigned char address);

/**
 * @function        CAN_check_for_data
 * @description     Checks if any of the 3 streams in the CAN bus contain data. The data will then be send via
                    a function pointer to be processed elsewhere
 * @author          
 * @date_created    
 * @date_modified   23/06/2018
 * @param   void
 * @returns void
 **/
void CAN_check_for_data();

#endif // CHASSIS_CAN_H