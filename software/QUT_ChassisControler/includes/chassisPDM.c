/**
 * @file chassisPDM.c
 * @author Jonn Dillon
 * @date 16/4/2018
 * @brief Handles functions, calls and utilities related to the Power Distribution Board
 **/

#include "chassisPDM.h"

uint8_t pdmRightTemp = 0;
uint8_t pdmLeftTemp = 0;

uint8_t PDM_convertADCToC(uint16_t val) {
    // Julius's old code...
    int i = 0;
    while (RADIATOR_CALIBRATION_VALUES[i] > val) i++;
    unsigned int j = ((RADIATOR_CALIBRATION_VALUES[i-1] - val) * 30);
    val = j / (RADIATOR_CALIBRATION_VALUES[i-1] - RADIATOR_CALIBRATION_VALUES[i]);
    return val + ((i * 3) + 25) * 10;
}

uint8_t PDM_getChassisTemps() {
    pdmRightTemp = PDM_convertADCToC(a2d_10bitCh(PDM_TEMP_RIGHT_CH));
    pdmLeftTemp = PDM_convertADCToC(a2d_10bitCh(PDM_TEMP_LEFT_CH));
    return 1;
}

uint8_t PDM_fanPumpCalc(
    uint8_t * fanRightSpeed,
    uint8_t * fanLeftSpeed,
    uint8_t * pumpRightSpeed,
    uint8_t * pumpLeftSpeed,
    uint8_t * inverterTemps
) {
    PDM_getChassisTemps(); // Pre-fills the global vars with the current temps of the temp sensors
    // Takes debug signal to force fans and pumps on
    if(debugPDMSig >= 1) { // Global value, defined in ChassisUART.h & .c
        *fanRightSpeed = 100;
        *fanLeftSpeed = 100;
        *pumpRightSpeed = 100;
        *pumpLeftSpeed = 100;
    } else { // If the debug signal is not forcing it on, calculate if it should be on

        // If either inverter temp is above 53c, pump on
        uint8_t inverterAboveTempRight = 0;
        uint8_t inverterAboveTempLeft = 0;
        // throw_error_code(ERROR_LEVEL_WARN, ERROR_BAD_CONFIG);
        if((CORE_invertersPresent & CORE_INVERTER_FRONT_LEFT) != 0) {
            // 0b00000110
            // 0b00000001
            // 0b00000000 0 == 0
            if((*inverterTemps + 0) > pdmTempSensorONTemp) {
                *pumpLeftSpeed = 100;
                inverterAboveTempLeft++;
            }
        }
        if((CORE_invertersPresent & CORE_INVERTER_FRONT_RIGHT) != 0) {
            // 0b00000110
            // 0b00000010
            // 0b00000010 2 != 0
            if((*inverterTemps + 0) > pdmTempSensorONTemp) {
                *pumpRightSpeed = 100;
                inverterAboveTempRight++;
            }
        }
        if((CORE_invertersPresent & CORE_INVERTER_BACK_RIGHT) != 0) {
            // 0b00000110
            // 0b00000100
            // 0b00000100 4 != 0
            if((*inverterTemps + 0) > pdmTempSensorONTemp) {
                *pumpRightSpeed = 100;
                inverterAboveTempRight++;
            }
        }
        if((CORE_invertersPresent & CORE_INVERTER_BACK_LEFT) != 0) {
            // 0b00000110
            // 0b00001000
            // 0b00000000 0 == 0
            if((*inverterTemps + 0) > pdmTempSensorONTemp) {
                *pumpLeftSpeed = 100;
                inverterAboveTempLeft++;
            }
        }

        // Only below if all inverter temps are below 50c, pump off
        if(inverterAboveTempRight > 0) {
            *pumpRightSpeed = 0;
        }
        if(inverterAboveTempLeft > 0) {
            *pumpLeftSpeed = 0;
        }


        if(pdmRightTemp >= pdmTempSensorONTemp) { // Fan on if the temp sensor is above 53c (Right)
            // Turn the right fan on
            *fanRightSpeed = 100;
        }
        if(pdmLeftTemp >= pdmTempSensorONTemp) { // Fan on if the temp sensor is above 53c (Left)
            // Turn the left fan on
            *fanLeftSpeed = 100;
        }

        if(pdmRightTemp <= pdmTempSensorOFFTemp) { // Fan off if the temp sensor is below 50c (Right)
            // Turn the right fan off
            *fanRightSpeed = 0;
        }
        if(pdmLeftTemp <= pdmTempSensorOFFTemp) { // Fan off if the temp sensor is below 50c (Left)
            // Turn the left fan off
            *fanLeftSpeed = 0;
        }
    }
    return 1;
}