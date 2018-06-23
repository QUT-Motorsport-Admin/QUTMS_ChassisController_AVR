/**
 * @file chassisPDM
 * @author Jonn Dillon
 * @date file creation 16/4/2018
 * @description This file defines all functions and operations related to the Power Distribution Unit
 **/

#ifndef CHASSIS_PDM_H
#define CHASSIS_PDM_H

#include <stdlib.h>
#include <avr/io.h>

#define PDM_BRAKELIGHT 		16			//byte 0
#define PDM_ATOMIC_ALARM	32			//byte 0
#define PDM_SHUTDOWN_PLUS	64			//byte 0
#define PDM_SHUTDOWN_MINUS	128			//byte 0
#define PDM_PRECHARGE		1			//byte 1

// Defines PDM struct
typedef struct PDM { uint8_t flags[4]; } PDM;
struct PDM pdm;

unsigned char PDM_data[4] = {2, 0, 0, 0};

#endif // CHASSIS_PDM_H