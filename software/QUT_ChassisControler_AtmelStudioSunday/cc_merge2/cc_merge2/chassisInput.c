/**
 * @file chassisInput.c
 * @author Jonn Dillon
 * @date 13/4/2018
 * @brief Manages inputs to car subsystems
 **/

#include "chassisInput.h"

uint16_t INPUT_ADC_ERROR = 40;                          /**< 3% Lower trim */
uint16_t INPUT_ADC_THRESH = 3;                          /**< 90% Upper trim */
uint16_t INPUT_BUFFER = 10;

// BRAKE PEDAL
uint16_t INPUT_PEDAL_BRAKE_CH1_HIGH =	  575;          /**< High Brake Pedal Application DONT SET, OVERWRIDDEN */
uint16_t INPUT_PEDAL_BRAKE_CH1_DIFF =	  250;          /**< Brake Pedal Travel Difference */
uint16_t INPUT_PEDAL_BRAKE_CH1_LOW =      385;          /**< Low Brake Pedal Application */
uint16_t INPUT_PEDAL_BRAKE_CH2_HIGH =     600;	        /**< High Brake Pedal Application DONT SET, OVERWRIDDEN */
uint16_t INPUT_PEDAL_BRAKE_CH2_DIFF =	  220;			/**< Brake Pedal Travel Difference */
uint16_t INPUT_PEDAL_BRAKE_CH2_LOW =      405;          /**< Low Brake Pedal Application */
uint16_t INPUT_PEDAL_BRAKE_LIGHT_ON =     10;           /**< Moderate Brake Pedal Application (7% Brake) */

// THROTTLE PEDAL
uint16_t INPUT_PEDAL_THROTTLE_CH1_HIGH =  490;	        /**< High Throttle Pedal Application DONT SET, OVERWRIDDEN */
uint16_t INPUT_PEDAL_THROTTLE_CH1_DIFF =  200;          /**< Brake Pedal Travel Difference */
uint16_t INPUT_PEDAL_THROTTLE_CH1_LOW =   310;	        /**< Low Throttle Pedal Application */
uint16_t INPUT_PEDAL_THROTTLE_CH2_HIGH =  460;          /**< High Throttle Pedal Application DONT SET, OVERWRIDDEN */
uint16_t INPUT_PEDAL_THROTTLE_CH2_DIFF =  200;          /**< Brake Pedal Travel Difference */
uint16_t INPUT_PEDAL_THROTTLE_CH2_LOW =   300;	        /**< Low Throttle Pedal Application */

// STEERING ANGLE
uint16_t INPUT_STEERING_RIGHT =			  900;          /**< Right Steering Application */
uint16_t INPUT_STEERING_LEFT =			  150;	        /**< Left Steering Application */

// BRAKE PRESSURE
uint16_t INPUT_PRESSURE_BRAKE_HIGH = 1022;              /**< High pressure lim in brake */
uint16_t INPUT_PRESSURE_BRAKE_LOW  = 1;                 /**< Low pressure lim in brake */

// PEDAL DELTA THRESHOLDS
uint16_t INPUT_PEDAL_DELTA_THRESH_H = 30;                /**< Low Value for pedal sensor discrepancy */
uint16_t INPUT_PEDAL_DELTA_THRESH_L = 00;                /**< Low Value for pedal sensor discrepancy */

uint8_t INPUT_steeringAngle = 0;
uint8_t INPUT_accelerationPedal = 0;
uint8_t INPUT_brakePedal = 0;
uint8_t INPUT_brakePressureFront = 0;
uint8_t INPUT_brakePressureBack = 0;

uint8_t INPUT_get_accelPedal(uint8_t *val) {
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

uint8_t INPUT_get_brakePedal(uint8_t *val) {
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

uint8_t INPUT_get_brakePressureFront(uint16_t *val) {
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

uint8_t INPUT_get_steeringWheel(uint8_t *val) {
	// Get Value
	uint16_t rawValue = 0;
	uint8_t state = INPUT_read_steeringWheel(&rawValue);
	// Convert Value
	*val = INPUT_scaleInput(
		&rawValue,
		INPUT_STEERING_RIGHT,
		INPUT_STEERING_LEFT
	);
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

uint8_t INPUT_get_brakePressureBack(uint16_t *val) {
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
uint8_t INPUT_scaleInput(uint16_t * value, uint16_t max, uint16_t min) {
	// High or low size filtering
	if(*value > max) return 100;
	if(*value < min) return 0;
    uint8_t tmp = (((*value - (min - INPUT_ADC_THRESH)) * 100) / ((max + INPUT_ADC_THRESH) - (min - INPUT_ADC_THRESH)));	
    return tmp;
}

/**
 * Example Code:
 * uint16_t val = 0;
 * if(INPUT_read_accelPedal(*val) == 0) {
 *     // Use val some way
 * } else {
 *     // Val is in error
 * }
 */
uint8_t INPUT_read_accelPedal(uint16_t *throttle) {

    //TODO: Fill buffers with int reads values
    static uint16_t primaryHistory[ADC_SAMPLES];
    static uint16_t secondaryHistory[ADC_SAMPLES];
    static uint8_t historyIndex = 0;

    // Read the values of the two throttle sensors and verify if the received values are valid
    primaryHistory[historyIndex] = a2d_10bitCh(INPUT_PEDAL_THROTTLE_CH1);
    secondaryHistory[historyIndex++] = a2d_10bitCh(INPUT_PEDAL_THROTTLE_CH2);

    if(historyIndex >= ADC_SAMPLES) { historyIndex = 0; }

    uint16_t primaryAverage = 0;
    uint16_t secondaryAverage = 0;
    for(uint8_t i = 0; i < ADC_SAMPLES; i++) {
        primaryAverage += primaryHistory[i];
        secondaryAverage += secondaryHistory[i];
    }
    primaryAverage /= ADC_SAMPLES;
    secondaryAverage /= ADC_SAMPLES;

    uint16_t delta = abs(primaryAverage - secondaryAverage); // Calculate the difference between the two values

    *throttle = primaryAverage; 

	if(primaryAverage < INPUT_PEDAL_THROTTLE_CH1_LOW - INPUT_ADC_ERROR ||
       secondaryAverage < INPUT_PEDAL_THROTTLE_CH2_LOW - INPUT_ADC_ERROR ) { return 1; }
	else if(primaryAverage > INPUT_PEDAL_THROTTLE_CH1_HIGH - INPUT_ADC_ERROR ||
            secondaryAverage > INPUT_PEDAL_THROTTLE_CH2_HIGH - INPUT_ADC_ERROR ) { return 2; }
	// Verify if the difference between sensors is within acceptable values
	else if(delta < INPUT_PEDAL_DELTA_THRESH_L ||
            delta > INPUT_PEDAL_DELTA_THRESH_H) { return 3; }
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
uint8_t INPUT_read_brakePedal(uint16_t * brake) {

    //TODO: Fill buffers with int reads values
    static uint16_t primaryHistory[ADC_SAMPLES];
    static uint16_t secondaryHistory[ADC_SAMPLES];
    static uint8_t historyIndex = 0;

	//uint8_t returnState = 0;
    // Read the values of the two throttle sensors and verify if the received values are valid
    primaryHistory[historyIndex] = a2d_10bitCh(INPUT_PEDAL_BRAKE_CH1);
    secondaryHistory[historyIndex++] = a2d_10bitCh(INPUT_PEDAL_BRAKE_CH2);

    if(historyIndex >= ADC_SAMPLES) { historyIndex = 0; }
	
    uint16_t primaryAverage = 0;
    uint16_t secondaryAverage = 0;
    for(uint8_t i = 0; i < ADC_SAMPLES; i++) {
        primaryAverage += primaryHistory[i];
        secondaryAverage += secondaryHistory[i];
    }
    primaryAverage /= ADC_SAMPLES;
    secondaryAverage /= ADC_SAMPLES;

    uint16_t delta = abs(primaryAverage - secondaryAverage); // Calculate the difference between the two values

    *brake = primaryAverage; 

	uint8_t returnState = 0;
	if(primaryAverage < (INPUT_PEDAL_BRAKE_CH1_LOW - INPUT_ADC_ERROR)) { returnState |= 1; }
	else if(primaryAverage > (INPUT_PEDAL_BRAKE_CH1_HIGH + INPUT_ADC_ERROR)) { returnState |= 2; }
	if(secondaryAverage < (INPUT_PEDAL_BRAKE_CH2_LOW - INPUT_ADC_ERROR)) { returnState |= 4; }
	else if(secondaryAverage > (INPUT_PEDAL_BRAKE_CH1_HIGH + INPUT_ADC_ERROR)) { returnState |= 8; }
	// Verify if the difference between sensors is within acceptable values
	if(delta < INPUT_PEDAL_DELTA_THRESH_L || delta > INPUT_PEDAL_DELTA_THRESH_H) { returnState |= 16; }  
	return returnState;
	
	//if(primaryAverage < INPUT_PEDAL_BRAKE_CH1_LOW - INPUT_ADC_ERROR ||
	//secondaryAverage < INPUT_PEDAL_BRAKE_CH2_LOW - INPUT_ADC_ERROR ) { return 1; }
	//else if(primaryAverage > INPUT_PEDAL_BRAKE_CH1_HIGH - INPUT_ADC_ERROR ||
	//secondaryAverage > INPUT_PEDAL_BRAKE_CH2_HIGH - INPUT_ADC_ERROR ) { return 2; }
	//// Verify if the difference between sensors is within acceptable values
	//else if(delta < INPUT_PEDAL_DELTA_THRESH_L ||
	//delta > INPUT_PEDAL_DELTA_THRESH_H) { return 3; }
	//return 0;
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
uint8_t INPUT_read_steeringWheel(uint16_t * steeringAngle) {

    //TODO: Fill buffers with int reads values
	uint8_t errorState = 0;
    static uint16_t history[ADC_SAMPLES];
    static uint8_t historyIndex = 0;

	//uint8_t returnState = 0;
	
    // Read the values of the two throttle sensors and verify if the received values are valid
    history[historyIndex++] = a2d_10bitCh(INPUT_STEERING_ANGLE_CH);

    if(historyIndex >= ADC_SAMPLES) { historyIndex = 0; }

    uint16_t average = 0;
    for(uint8_t i = 0; i < ADC_SAMPLES; i++) {
        average += history[i];
    }
    average /= ADC_SAMPLES;
	*steeringAngle = average;
	
	if(average < (INPUT_STEERING_LEFT - INPUT_ADC_ERROR)) { errorState += 1; } // Check if the value we received is valid
    if(average > (INPUT_STEERING_RIGHT + INPUT_ADC_ERROR)) { errorState += 2; } // Check if the value we received is valid
	return errorState;
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
uint8_t INPUT_read_brakePressureFront(uint16_t * fntPressure) {

    //TODO: Fill buffers with int reads values
    static uint16_t history[ADC_SAMPLES];
    static uint8_t historyIndex = 0;

    // Read the values of the two throttle sensors and verify if the received values are valid
    history[historyIndex] = a2d_10bitCh(INPUT_PRESSURE_BRAKE_FRONT);

    if(historyIndex >= ADC_SAMPLES) { historyIndex = 0; }

    uint16_t average = 0;
    for(uint8_t i = 0; i < ADC_SAMPLES; i++) {
        average += history[i];
    }
    average /= ADC_SAMPLES;

	if(average < INPUT_PRESSURE_BRAKE_LOW) { return 1; } // Check if the value we received is valid
    if(average > INPUT_PRESSURE_BRAKE_HIGH) { return 2; } // Check if the value we received is valid
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
uint8_t INPUT_read_brakePressureBack(uint16_t * bkPressure) {

    //TODO: Fill buffers with int reads values
    static uint16_t history[10];
    static uint8_t historyIndex = 0;

    // Read the values of the two throttle sensors and verify if the received values are valid
    history[historyIndex] = a2d_10bitCh(INPUT_PRESSURE_BRAKE_BACK);

    if(historyIndex >= ADC_SAMPLES) { historyIndex = 0; }

    uint16_t average = 0;
    for(uint8_t i = 0; i < ADC_SAMPLES; i++) {
        average += history[i];
    }
    average /= ADC_SAMPLES;

	if(average < INPUT_PRESSURE_BRAKE_LOW) { return 1; } // Check if the value we received is valid
    if(average > INPUT_PRESSURE_BRAKE_HIGH) { return 2; } // Check if the value we received is valid
	return 0;
}

/*
 * Point of this function is to deal with the ever increasing shift in the input pots
 * Will set the lowest to what its currently sensing plus a buffer
 */
void INPUT_init_input() {
	
	uint16_t primaryHistory[ADC_SAMPLES];
	uint16_t secondaryHistory[ADC_SAMPLES];
	uint16_t primaryAverage = 0;
	uint16_t secondaryAverage = 0;
	
	
	// BRAKE SETTING
	
	for(uint8_t i = 0; i < ADC_SAMPLES; i++) {
		primaryHistory[i] = a2d_10bitCh(INPUT_PEDAL_BRAKE_CH1);
		secondaryHistory[i] = a2d_10bitCh(INPUT_PEDAL_BRAKE_CH2);
	}
	
	for(uint8_t i = 0; i < ADC_SAMPLES; i++) {
		primaryAverage += primaryHistory[i];
		secondaryAverage += secondaryHistory[i];
	}
	primaryAverage /= ADC_SAMPLES;
	secondaryAverage /= ADC_SAMPLES;
	
	INPUT_PEDAL_BRAKE_CH1_LOW = primaryAverage + INPUT_BUFFER;
	INPUT_PEDAL_BRAKE_CH2_LOW = secondaryAverage + INPUT_BUFFER;
	INPUT_PEDAL_BRAKE_CH1_HIGH = primaryAverage + INPUT_BUFFER + INPUT_PEDAL_BRAKE_CH1_DIFF;
	INPUT_PEDAL_BRAKE_CH2_HIGH = secondaryAverage + INPUT_BUFFER + INPUT_PEDAL_BRAKE_CH2_DIFF;
	
	
	// THROTTLE SETTING

	for(uint8_t i = 0; i < ADC_SAMPLES; i++) {
		primaryHistory[i] = a2d_10bitCh(INPUT_PEDAL_THROTTLE_CH1);
		secondaryHistory[i] = a2d_10bitCh(INPUT_PEDAL_THROTTLE_CH2);
	}
	
	for(uint8_t i = 0; i < ADC_SAMPLES; i++) {
		primaryAverage += primaryHistory[i];
		secondaryAverage += secondaryHistory[i];
	}
	primaryAverage /= ADC_SAMPLES;
	secondaryAverage /= ADC_SAMPLES;
	
	INPUT_PEDAL_THROTTLE_CH1_LOW = primaryAverage + INPUT_BUFFER;
	INPUT_PEDAL_THROTTLE_CH2_LOW = secondaryAverage + INPUT_BUFFER;
	INPUT_PEDAL_THROTTLE_CH1_HIGH = primaryAverage + INPUT_BUFFER + INPUT_PEDAL_THROTTLE_CH1_DIFF;
	INPUT_PEDAL_THROTTLE_CH2_HIGH = secondaryAverage + INPUT_BUFFER + INPUT_PEDAL_THROTTLE_CH2_DIFF;
}