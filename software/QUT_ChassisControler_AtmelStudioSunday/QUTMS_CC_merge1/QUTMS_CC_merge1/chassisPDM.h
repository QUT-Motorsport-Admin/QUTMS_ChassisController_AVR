/**
 * @file chassisPDM.h
 * @author Jonn Dillon
 * @date file creation 16/4/2018
 * @brief This file defines all functions and operations related to the Power Distribution Unit
 **/

#ifndef CHASSIS_PDM_H
#define CHASSIS_PDM_H

#include <stdlib.h>
#include <avr/io.h>

#define PDM_BRAKELIGHT 		16 /**< Byte 0 */
#define PDM_ATOMIC_ALARM	32 /**< Byte 0 */
#define PDM_SHUTDOWN_PLUS	64 /**< Byte 0 */
#define PDM_SHUTDOWN_MINUS	128 /**< Byte 0 */
#define PDM_PRECHARGE		1 /**< Byte 1 */

/**
 * @brief Data Structure for PDM Information
 * 
 */
typedef struct PDM { uint8_t flags[4]; } PDM;

/**
 * @brief Central storage for PDM Data
 * 
 */
struct PDM pdm;

/**
 * @brief Appears unused?
 * 
 */
unsigned char PDM_data[4] = {2, 0, 0, 0};

/**
 * @brief Precharge the motor controllers and then enable relays to allow the starting of the car
 * 
 */
void pdm_init();

#endif // CHASSIS_PDM_H