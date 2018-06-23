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

// ===================================== FUNCTION DECLARATIONS =====================================

// TODO: Comment when functions are finalised
void CAN_send(uint8_t CANbus, uint8_t numBytes, uint8_t * data, uint32_t ID);
uint32_t CAN_ID_constructor(uint32_t sendingID, unsigned char type, unsigned char address, uint32_t status);
void CAN_pull_packet(uint8_t CANbus, uint8_t* numBytes, uint8_t* data, uint32_t* ID);

#endif // CHASSIS_CAN_H