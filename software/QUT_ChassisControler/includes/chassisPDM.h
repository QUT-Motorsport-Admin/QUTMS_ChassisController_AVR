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
#include "utils/a2d.h"
#include "includes/chassisUART.h"
#include "includes/chassisError.h"
#include "main.h"

#define PDM_TEMP_RIGHT_CH   (0) // Left side temperature sensor     - ADC11
#define PDM_TEMP_LEFT_CH    (2) // Right side temperature sensor    - ADC12

extern uint8_t pdmRightTemp;
extern uint8_t pdmLeftTemp;

const uint8_t pdmInverterONTemp =       53;
const uint8_t pdmInverterOFFTemp =      50;
const uint8_t pdmTempSensorONTemp =     53;
const uint8_t pdmTempSensorOFFTemp =    50;

const uint16_t RADIATOR_CALIBRATION_VALUES[27] = { // radiator_cals_acewell_22k
    801, 800, 799, 797, 791, 
    785, 767, 750, 734, 707, 
    689, 671, 637, 598, 581, 
    562, 529, 493, 464, 443, 
    359, 338, 317, 297, 278, 
    234, 204
};

#endif // CHASSIS_PDM_H