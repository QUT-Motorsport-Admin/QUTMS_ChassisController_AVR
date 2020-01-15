/**
 * @file chassisInput.c
 * @author Jonn Dillon
 * @date 13/4/2018
 * @brief Manages inputs to car subsystems
 **/

#include "chassisInput.h"

const uint16_t INPUT_ADC_ERROR = 40;                          /**< 3% Lower trim */
const uint16_t INPUT_ADC_THRESH = 3;                          /**< 90% Upper trim */
const uint16_t INPUT_BUFFER = 10;

// BRAKE PEDAL
const uint16_t INPUT_PEDAL_BRAKE_CH1_HIGH	=	440;	/// Maximum brake pedal input
const uint16_t INPUT_PEDAL_BRAKE_CH1_DIFF	=	250;	/// Brake pedal input difference
const uint16_t INPUT_PEDAL_BRAKE_CH1_LOW	=	350;	/// Minimum brake pedal input
const uint16_t INPUT_PEDAL_BRAKE_CH2_HIGH	=	430;	/// Maximum brake pedal input
const uint16_t INPUT_PEDAL_BRAKE_CH2_DIFF	=	220;	/// Brake pedal input difference
const uint16_t INPUT_PEDAL_BRAKE_CH2_LOW	=	340;	/// Minimum brake pedal input
const uint16_t INPUT_PEDAL_BRAKE_LIGHT_ON	=	10;		/// Brake light illumination threshold

// THROTTLE PEDAL
const uint16_t INPUT_PEDAL_THROTTLE_CH1_HIGH	=	465;	/// Maximum brake pedal input
const uint16_t INPUT_PEDAL_THROTTLE_CH1_DIFF	=	200;	/// Throttle pedal input difference
const uint16_t INPUT_PEDAL_THROTTLE_CH1_LOW		=	290;	/// Minimum brake pedal input
const uint16_t INPUT_PEDAL_THROTTLE_CH2_HIGH	=	535;	/// Maximum brake pedal input
const uint16_t INPUT_PEDAL_THROTTLE_CH2_DIFF	=	200;	/// Throttle pedal input difference
const uint16_t INPUT_PEDAL_THROTTLE_CH2_LOW		=	345;	/// Minimum brake pedal input

// STEERING ANGLE
const uint16_t INPUT_STEERING_RIGHT	=	800;	/// Right lock steering input
const uint16_t INPUT_STEERING_LEFT	=	150;	/// Left lock steering input

// BRAKE PRESSURE
const uint16_t INPUT_PRESSURE_BRAKE_HIGH	=	1022;	/// Maximum brake pressure input
const uint16_t INPUT_PRESSURE_BRAKE_LOW		=	1;		/// Minimum brake pressure input

// PEDAL DELTA THRESHOLDS
const uint16_t INPUT_PEDAL_DELTA_THRESH_H = 1023;	/// Maximum pedal input difference
const uint16_t INPUT_PEDAL_DELTA_THRESH_L = 0;		/// Minimum pedal input difference

int8_t INPUT_steeringAngle = 0;
uint8_t INPUT_accelerationPedal = 0;
uint8_t INPUT_brakePedal = 0;
uint8_t INPUT_brakePressureFront = 0;
uint8_t INPUT_brakePressureBack = 0;

uint8_t INPUT_get_accelPedal(uint8_t* val) {
    // Get Value
    uint16_t rawValue = 0;
    uint8_t state = INPUT_read_accelPedal(&rawValue);

    // Convert Value
    *val = INPUT_scaleInput(
               &rawValue,
               INPUT_PEDAL_THROTTLE_CH1_HIGH,
               INPUT_PEDAL_THROTTLE_CH1_LOW
           );

    // Error States
    switch (state) {
    case 1: // Inputs were too low
        throw_error_code(ERROR_LEVEL_WARN, ERROR_THROTTLE_ERROR);
        break;
    case 2: // Inputs were too high
        throw_error_code(ERROR_LEVEL_WARN, ERROR_THROTTLE_ERROR);
        break;
    case 3: // Delta was found to be in failure
        throw_error_code(ERROR_LEVEL_WARN, ERROR_THROTTLE_ERROR);
        break;
    default:
        // Do nothing
        break;
    }

    return state; // Pass up state of read for process and logic use
}

uint8_t INPUT_get_brakePedal(uint8_t* val) {
    // Get Value
    uint16_t rawValue = 0;
    uint8_t state = INPUT_read_brakePedal(&rawValue);
    // Convert Value
    *val = INPUT_scaleInput(
               &rawValue,
               INPUT_PEDAL_BRAKE_CH1_HIGH,
               INPUT_PEDAL_BRAKE_CH1_LOW
           );
    // Error States
    switch (state) {
    case 1: // Inputs were too low
        throw_error_code(ERROR_LEVEL_WARN, ERROR_BRAKES_ERROR);
        break;
    case 2: // Inputs were too high
        throw_error_code(ERROR_LEVEL_WARN, ERROR_BRAKES_ERROR);
        break;
    case 3: // Delta was found to be in failure
        throw_error_code(ERROR_LEVEL_WARN, ERROR_BRAKES_ERROR);
        break;
    default:
        // Do nothing
        break;
    }
    return state; // Pass up state of read for process and logic use
}

uint8_t INPUT_get_brakePressureFront(uint16_t* val) {
    uint8_t state = INPUT_read_brakePressureFront(val);
    switch (state) {
    case 1: // Inputs were too low
        throw_error_code_with_number(ERROR_LEVEL_WARN, ERROR_BRAKES_PRESSURE_LOW, 1);
        break;
    case 2: // Inputs were too high
        throw_error_code_with_number(ERROR_LEVEL_WARN, ERROR_BRAKES_PRESSURE_HIGH, 1);
        break;
    case 3: // Delta was found to be in failure
        throw_error_code_with_number(ERROR_LEVEL_WARN, ERROR_BRAKES_PRESSURE, 1);
        break;
    default:
        // Do nothing
        break;
    }
    return state; // Pass up state of read for process and logic use
}

uint8_t INPUT_get_steeringWheel(int8_t* val) {
    // Get Value
    uint16_t rawValue = 0;
    uint8_t state = INPUT_read_steeringWheel(&rawValue);
    // Convert Value

    uint8_t tempVal = 2 * INPUT_scaleInput(
                          &rawValue,
                          INPUT_STEERING_RIGHT,
                          INPUT_STEERING_LEFT
                      ); // go from 0 to 100 to 0 to 200

    // go from 0 to 200 to -100 to 100
    *val = tempVal - 100;

    // Error States
    switch (state) {
    case 1: // Inputs were too low
        throw_error_code(ERROR_LEVEL_WARN, ERROR_STEERING_ANGLE_RIGHT);
        break;
    case 2: // Inputs were too high
        throw_error_code(ERROR_LEVEL_WARN, ERROR_STEERING_ANGLE_LEFT);
        break;
    default:
        // Do nothing
        break;
    }
    return state; // Pass up state of read for process and logic use
}

uint8_t INPUT_get_brakePressureBack(uint16_t* val) {
    uint8_t state = INPUT_read_brakePressureFront(val);
    switch (state) {
    case 1: // Inputs were too low
        throw_error_code_with_number(ERROR_LEVEL_WARN, ERROR_BRAKES_PRESSURE_LOW, 2);
        break;
    case 2: // Inputs were too high
        throw_error_code_with_number(ERROR_LEVEL_WARN, ERROR_BRAKES_PRESSURE_HIGH, 2);
        break;
    case 3: // Delta was found to be in failure
        throw_error_code_with_number(ERROR_LEVEL_WARN, ERROR_BRAKES_PRESSURE, 2);
        break;
    default:
        // Do nothing
        break;
    }
    return state; // Pass up state of read for process and logic use
}

/**
 * @brief
 *
 * @param value
 * @param max
 * @param min
 * @return uint8_t
 */
uint8_t INPUT_scaleInput(uint16_t* value, uint16_t max, uint16_t min) {
    // High or low size filtering
    if(*value > max) return 100;
    if(*value < min) return 0;
    uint8_t tmp = (((*value - (min - INPUT_ADC_THRESH)) * 100) / ((max + INPUT_ADC_THRESH) - (min - INPUT_ADC_THRESH)));
    return tmp;
}

/**
 * @brief Reads the inputs from the ADCs and checks for faults with the given input values
 *
 * @param *throttle Pointer to the throttle value
 * @returns Error state
 */
uint8_t INPUT_read_accelPedal(uint16_t* throttle) {

    *throttle = 0;

    const uint16_t channel1 = a2d_10bitCh(INPUT_PEDAL_THROTTLE_CH1);

    if (channel1 < (INPUT_PEDAL_THROTTLE_CH1_LOW - 10) || channel1 > (INPUT_PEDAL_THROTTLE_CH1_HIGH + 10))
        return 1; // outside allowable range

    const uint16_t channel2 = a2d_10bitCh(INPUT_PEDAL_THROTTLE_CH2);

    if (channel2 < (INPUT_PEDAL_THROTTLE_CH2_LOW - 10) || channel2 > (INPUT_PEDAL_THROTTLE_CH2_HIGH + 10))
        return 1; // outside allowable range

    const uint16_t delta = abs(channel1 - channel2);

    if (delta < INPUT_PEDAL_DELTA_THRESH_L || delta > INPUT_PEDAL_DELTA_THRESH_H)
        return 1; // potentiometer difference is not within range

    *throttle = channel1;
    return 0;
}

/**
 * Example Code:
 * uint16_t val = 0;
 * if(INPUT_read_brakePedal(*val) == 0) {
 *     // Use val some way
 * } else {
 *     // Val is in error
 * }
 */
uint8_t INPUT_read_brakePedal(uint16_t* brake) {

    *brake = 0;

    const uint16_t channel1 = a2d_10bitCh(INPUT_PEDAL_BRAKE_CH1);

    if (channel1 < (INPUT_PEDAL_BRAKE_CH1_LOW - 10) || channel1 > (INPUT_PEDAL_BRAKE_CH1_HIGH + 10))
        return 1; // outside allowable range

    const uint16_t channel2 = a2d_10bitCh(INPUT_PEDAL_BRAKE_CH2);

    if (channel2 < (INPUT_PEDAL_BRAKE_CH2_LOW - 10) || channel2 > (INPUT_PEDAL_BRAKE_CH2_HIGH + 10))
        return 1; // outside allowable range

    const uint16_t delta = abs(channel1 - channel2);

    if (delta < INPUT_PEDAL_DELTA_THRESH_L || delta > INPUT_PEDAL_DELTA_THRESH_H)
        return 1; // potentiometer difference is not within range

    *brake = channel1;
    return 0;
}

/**
 * Example Code:
 * uint16_t val = 0;
 * if(INPUT_read_brakePressureFront(*val) == 0) {
 *     // Use val some way
 * } else {
 *     // Val is in error
 * }
 */
uint8_t INPUT_read_steeringWheel(uint16_t* steeringAngle) {

    *steeringAngle = 0;

    uint16_t channel = a2d_10bitCh(INPUT_STEERING_ANGLE_CH);

    if (channel < (INPUT_STEERING_LEFT - INPUT_ADC_ERROR) || channel > (INPUT_STEERING_RIGHT + INPUT_ADC_ERROR))
        return 1; // outside allowable range

    *steeringAngle = channel;
    return 0;
}

/**
 * Example Code:
 * uint16_t val = 0;
 * if(INPUT_read_brakePressureFront(*val) == 0) {
 *     // Use val some way
 * } else {
 *     // Val is in error
 * }
 */
uint8_t INPUT_read_brakePressureFront(uint16_t* fntPressure) {

    //TODO: Fill buffers with int reads values
    static uint16_t history[ADC_SAMPLES];
    static uint8_t historyIndex = 0;

    // Read the values of the two throttle sensors and verify if the received values are valid
    history[historyIndex] = a2d_10bitCh(INPUT_PRESSURE_BRAKE_FRONT);

    if(historyIndex >= ADC_SAMPLES) {
        historyIndex = 0;
    }

    uint16_t average = 0;
    for(uint8_t i = 0; i < ADC_SAMPLES; i++) {
        average += history[i];
    }
    average /= ADC_SAMPLES;

    if(average < INPUT_PRESSURE_BRAKE_LOW) {
        return 1;    // Check if the value we received is valid
    }
    if(average > INPUT_PRESSURE_BRAKE_HIGH) {
        return 2;    // Check if the value we received is valid
    }
    return 0;
}

/**
 * Example Code:
 * uint16_t val = 0;
 * if(INPUT_read_breakPressureBack(*val) == 0) {
 *     // Use val some way
 * } else {
 *     // Val is in error
 * }
 */
uint8_t INPUT_read_brakePressureBack(uint16_t* bkPressure) {

    //TODO: Fill buffers with int reads values
    static uint16_t history[10];
    static uint8_t historyIndex = 0;

    // Read the values of the two throttle sensors and verify if the received values are valid
    history[historyIndex] = a2d_10bitCh(INPUT_PRESSURE_BRAKE_BACK);

    if(historyIndex >= ADC_SAMPLES) {
        historyIndex = 0;
    }

    uint16_t average = 0;
    for(uint8_t i = 0; i < ADC_SAMPLES; i++) {
        average += history[i];
    }
    average /= ADC_SAMPLES;

    if(average < INPUT_PRESSURE_BRAKE_LOW) {
        return 1;    // Check if the value we received is valid
    }
    if(average > INPUT_PRESSURE_BRAKE_HIGH) {
        return 2;    // Check if the value we received is valid
    }
    return 0;
}
