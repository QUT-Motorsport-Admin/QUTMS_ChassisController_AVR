/**
 * @file chassisInput.c
 * @author Jonn Dillon
 * @date 13/4/2018
 * @brief Manages inputs to car subsystems
 **/

#include "chassisInput.h"

uint8_t INPUT_steeringAngle = 0;
uint8_t INPUT_accelerationPedal = 0;
uint8_t INPUT_brakePedal = 0;
uint8_t INPUT_brakePressureFront = 0;
uint8_t INPUT_brakePressureBack = 0;

#define ADC_SAMPLES	(8) /**< Samples required for ADC */

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
            // PORTK = 0b00100000;
            throw_error_code(ERROR_LEVEL_WARN, ERROR_THROTTLE_ERROR);
            break;
        case 2: // Inputs were too high
            // PORTK = 0b00100000;
            throw_error_code(ERROR_LEVEL_WARN, ERROR_THROTTLE_ERROR);
            break;
        case 3: // Delta was found to be in failure
            // PORTK = 0b00100000;
            throw_error_code(ERROR_LEVEL_WARN, ERROR_THROTTLE_ERROR);
            break;
        default:
            // Do nothing
            // PORTK = 0b00100000;
            break;
    }
    return state; // Pass up state of read for process and logic use
}

uint8_t INPUT_get_brakePedal(uint8_t *val) {
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
            throw_error_code(ERROR_LEVEL_WARN, ERROR_BRAKES_POSITION_LOW);
            break;
        case 2: // Inputs were too high
            throw_error_code(ERROR_LEVEL_WARN, ERROR_BRAKES_POSITION_HIGH);
            break;
        case 3: // Delta was found to be in failure
            throw_error_code(ERROR_LEVEL_WARN, ERROR_BRAKES_POSITION);
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
    uint8_t tmp = *value / ((max + INPUT_ADC_THRESH)-(min - INPUT_ADC_THRESH)) * 100;
    return tmp > 100 ? 100 : tmp;
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
    static uint16_t primaryHistory[10];
    static uint16_t secondaryHistory[10];
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

	if(primaryAverage < INPUT_PEDAL_THROTTLE_CH1_LOW - INPUT_ADC_ERROR || secondaryAverage < INPUT_PEDAL_THROTTLE_CH2_LOW - INPUT_ADC_ERROR ) 
    { 
        return 1; 
    }
	else if(primaryAverage > INPUT_PEDAL_THROTTLE_CH1_HIGH - INPUT_ADC_ERROR || secondaryAverage > INPUT_PEDAL_THROTTLE_CH2_HIGH - INPUT_ADC_ERROR )
    {
        return 2; 
    }
	// Verify if the difference between sensors is within acceptable values
	else if(delta < INPUT_PEDAL_DELTA_THRESH_L || delta > INPUT_PEDAL_DELTA_THRESH_H)
    {
        return 3;
    }
	else 
    {
        return 0; 
    }
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
    // Read the values of the two brake sensors and verify if the received values are valid
	uint16_t primary = adc_read_avg(INPUT_PEDAL_BRAKE_CH1);
    uint16_t secondary = adc_read_avg(INPUT_PEDAL_BRAKE_CH2);
    uint16_t delta = abs(primary-secondary); // Calculate the difference between the two values
    *brake = primary; 
	if(primary < INPUT_PEDAL_BRAKE_CH1_LOW || secondary < INPUT_PEDAL_BRAKE_CH2_LOW) { return 1; }
    if(primary > INPUT_PEDAL_BRAKE_CH1_HIGH || secondary > INPUT_PEDAL_BRAKE_CH2_HIGH) { return 2; }
	// Verify if the difference between sensors is within acceptable values
	if(delta < INPUT_PEDAL_DELTA_THRESH_L || delta > INPUT_PEDAL_DELTA_THRESH_H) { return 3; }
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
uint8_t INPUT_read_brakePressureFront(uint16_t * fntPressure) {
    uint16_t tmp = adc_read_avg(INPUT_PRESSURE_BRAKE_FRONT); // Get the pressure in the front brake
    *fntPressure = tmp; // Still gets the value found regardless
	if(tmp < INPUT_PRESSURE_BRAKE_LOW) { return 1; } // Check if the value we received is valid
    if(tmp > INPUT_PRESSURE_BRAKE_HIGH) { return 2; } // Check if the value we received is valid
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
uint8_t INPUT_read_breakPressureBack(uint16_t * bkPressure) {
    uint16_t tmp = adc_read_avg(INPUT_PRESSURE_BRAKE_BACK); // Get the pressure in the front brake
    *bkPressure = tmp; // Still gets the value found regardless
	if(tmp < INPUT_PRESSURE_BRAKE_LOW) { return 1; } // Check if the value we received is valid
    if(tmp > INPUT_PRESSURE_BRAKE_HIGH) { return 2; } // Check if the value we received is valid
	return 0;
}


// OLD ------------------------------------------------------------------------

/**
 * getTorqueDemand
 * Input:	none
 * Returns: returns the current torque command value as a percentage between 0 and 100, -1 for any errors
 * 
 * Reference: 	ATmega Datasheet Chapter 13 (I/O-Ports)
 * 				ATmega Datasheet Chapter 26 (ADC - Analog to Digital Converter)
 **/
// int8_t input_get_torque_percent()
// {
//     unsigned int currentTorqueDemand[4] = {0, 0, 0, 0};
// 	unsigned int AN1_voltage = a2d_10bitCh(4);
// 	unsigned int AN2_voltage = a2d_10bitCh(3);
// 	unsigned int temp_currentTorqueDemand = AN1_voltage / 4;
// 	if (temp_currentTorqueDemand > 256) temp_currentTorqueDemand = 0;
// 	if (temp_currentTorqueDemand < 10) temp_currentTorqueDemand = 0;
// 	currentTorqueDemand[0] = temp_currentTorqueDemand;
	
// 	if(currentTorqueDemand[0] > 250) PORTA |= 32;
// 	else PORTA &= 223;
// }

/**
 * pressure_brake_read()
 * Input:	front	-	A pointer to the variable that holds the digital value of the front brake pressure
 * 			rear	-	A pointer to the variable that holds the digital value of the rear brake pressure
 * Returns: 0 if any of the brakes' pressure readings fall outside the bounds specified by PRESSURE_BRAKE_LOW
 * 			and PRESSURE_BRAKE_HIGH. 1 if the pressures are inside the bounds.
 * 
 * Reference: ATmega Datasheet Chapter 26 (ADC - Analog to Digital Converter)
 **/
// uint8_t input_get_brake_percent(uint16_t * front, uint16_t * rear)
// {
// 	uint16_t tmp = 0;
// 	tmp = adc_read_avg(PRESSURE_BRAKE_FRONT);							// Get the pressure in the front brake
// 	if(tmp < PRESSURE_BRAKE_LOW || tmp > PRESSURE_BRAKE_HIGH)return 0;	// Check if the value we received is valid
// 	*front = tmp;
	
// 	tmp = adc_read_avg(PRESSURE_BRAKE_REAR);							// Get the pressure in the rear brake
// 	if(tmp < PRESSURE_BRAKE_LOW || tmp > PRESSURE_BRAKE_HIGH)return 0;	// Check if the value we received is valid
// 	*rear = tmp;
	
// 	return 1;
// }

/**
 * pedal_read()
 * Input:	brake		-	A pointer to a variable holding the value of the brake sensor
 * 			throttle	-	A pointer to a variable holding the value of the throttle sensor
 * Returns: 0 if any of the sensor data is invalid or if there is a disparity above acceptable limits in the values read 
 * 			by the two sensors in each device
 * 
 * The throttle and the brake contain two sensors each. The data from each sensor is gathered and made sure it is valid and
 * similar to it's partner sensor.
 * If the values of the brake sensors are large enough, the brake light will turn on.
 * 
 * Reference: ATmega Datasheet Chapter 26 (ADC - Analog to Digital Converter)
 **/
// uint8_t pedal_read(uint16_t * brake, uint16_t * throttle)
// {
// 	uint16_t primary = 0;	
// 	uint16_t secondary = 0;
// 	int16_t delta = 0;
	
// 	// Read the values of the two brake sensors and verify if the received values are valid
// 	primary = adc_read_avg(PEDAL_BRAKE_CH1);
// 	if(primary < INPUT_PEDAL_BRAKE_LOW || primary > INPUT_PEDAL_BRAKE_HIGH)return 0;
// 	secondary = adc_read_avg(PEDAL_BRAKE_CH2);
// 	if(secondary < INPUT_PEDAL_BRAKE_LOW || secondary > INPUT_PEDAL_BRAKE_HIGH)return 0;
// 	// Calculate the difference between the two values
// 	delta = abs(primary-secondary);
// 	// Verify if the difference between sensors is within acceptable values
// 	if(delta > INPUT_PEDAL_DELTA_THRESH_L && delta < INPUT_PEDAL_DELTA_THRESH_H)
// 	{
// 		*brake = primary;
// 		if(*brake > INPUT_PEDAL_BRAKE_LIGHT_ON)
// 		{
// 			//check if the brake light is already on.
// 			if((pdm.flags[0] & PDM_BRAKELIGHT) == 0 )
// 			{
// 				//if not, set it and send the can packet immediately
// 				pdm.flags[0] |= PDM_BRAKELIGHT;
// 				CAN_send_heartbeat(PDM_H, NORMAL, 1);
// 			}
// 		}
// 	}
// 	else return 0;
	
// 	// Read the values of the two throttle sensors and verify if the received values are valid
// 	primary = adc_read_avg(PEDAL_THROTTLE_CH1);
// 	if(primary < INPUT_PEDAL_THROTTLE_LOW || primary > INPUT_PEDAL_THROTTLE_HIGH)return 0;
// 	secondary = adc_read_avg(PEDAL_THROTTLE_CH2);
// 	if(secondary < INPUT_PEDAL_THROTTLE_LOW || secondary > INPUT_PEDAL_THROTTLE_HIGH)return 0;
// 	// Calculate the difference between the two values
// 	delta = abs(primary-secondary);
// 	// Verify if the difference between sensors is within acceptable values
// 	if(delta > INPUT_PEDAL_DELTA_THRESH_L && delta < INPUT_PEDAL_DELTA_THRESH_H)
// 	{
// 		*throttle = primary;
// 	}
// 	else return 0;
	
// 	return 1;
// }