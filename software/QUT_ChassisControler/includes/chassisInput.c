/**
 * @file chassisInput
 * @author Jonn Dillon
 * @date 13/4/2018
 * @description 
 **/


// OLD ------------------------------------------------------------------------

/**
 * pressure_brake_read()
 * Input:	front	-	A pointer to the variable that holds the digital value of the front brake pressure
 * 			rear	-	A pointer to the variable that holds the digital value of the rear brake pressure
 * Returns: 0 if any of the brakes' pressure readings fall outside the bounds specified by PRESSURE_BRAKE_LOW
 * 			and PRESSURE_BRAKE_HIGH. 1 if the pressures are inside the bounds.
 * 
 * Reference: ATmega Datasheet Chapter 26 (ADC - Analog to Digital Converter)
 **/
uint8_t pressure_brake_read(uint16_t * front, uint16_t * rear)
{
	uint16_t tmp = 0;
	tmp = adc_read_avg(PRESSURE_BRAKE_FRONT);							// Get the pressure in the front brake
	if(tmp < PRESSURE_BRAKE_LOW || tmp > PRESSURE_BRAKE_HIGH)return 0;	// Check if the value we received is valid
	*front = tmp;
	
	tmp = adc_read_avg(PRESSURE_BRAKE_REAR);							// Get the pressure in the rear brake
	if(tmp < PRESSURE_BRAKE_LOW || tmp > PRESSURE_BRAKE_HIGH)return 0;	// Check if the value we received is valid
	*rear = tmp;
	
	return 1;
}

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
uint8_t pedal_read(uint16_t * brake, uint16_t * throttle)
{
	uint16_t primary = 0;	
	uint16_t secondary = 0;
	int16_t delta = 0;
	
	// Read the values of the two brake sensors and verify if the received values are valid
	primary = adc_read_avg(PEDAL_BRAKE_CH1);
	if(primary < PEDAL_BRAKE_LOW || primary > PEDAL_BRAKE_HIGH)return 0;
	secondary = adc_read_avg(PEDAL_BRAKE_CH2);
	if(secondary < PEDAL_BRAKE_LOW || secondary > PEDAL_BRAKE_HIGH)return 0;
	// Calculate the difference between the two values
	delta = abs(primary-secondary);
	// Verify if the difference between sensors is within acceptable values
	if(delta > PEDAL_DELTA_THRESH_L && delta < PEDAL_DELTA_THRESH_H)
	{
		*brake = primary;
		if(*brake > PEDAL_BRAKE_LIGHT_ON)
		{
			//check if the brake light is already on.
			if((pdm.flags[0] & PDM_BRAKELIGHT) == 0 )
			{
				//if not, set it and send the can packet immediately
				pdm.flags[0] |= PDM_BRAKELIGHT;
				send_heartbeat(PDM_H, NORMAL, 1);
			}
		}
	}
	else return 0;
	
	// Read the values of the two throttle sensors and verify if the received values are valid
	primary = adc_read_avg(PEDAL_THROTTLE_CH1);
	if(primary < PEDAL_THROTTLE_LOW || primary > PEDAL_THROTTLE_HIGH)return 0;
	secondary = adc_read_avg(PEDAL_THROTTLE_CH2);
	if(secondary < PEDAL_THROTTLE_LOW || secondary > PEDAL_THROTTLE_HIGH)return 0;
	// Calculate the difference between the two values
	delta = abs(primary-secondary);
	// Verify if the difference between sensors is within acceptable values
	if(delta > PEDAL_DELTA_THRESH_L && delta < PEDAL_DELTA_THRESH_H)
	{
		*throttle = primary;
	}
	else return 0;
	
	return 1;
}