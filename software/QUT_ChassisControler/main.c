#include "utils/SPI.h"
#include "utils/MCP2515.h"
#include "utils/uart.h"
#include "utils/a2d.h"

/**
 * torque_calculate_current_demand()
 * Input:	none
 * Returns: none
 * 
 * 
 * 
 * Reference: 	ATmega Datasheet Chapter 13 (I/O-Ports)
 * 				ATmega Datasheet Chapter 26 (ADC - Analog to Digital Converter)
 **/
void torque_calculate_current_demand()
{
    unsigned int currentTorqueDemand[4] = {0, 0, 0, 0};
	unsigned int AN1_voltage = a2d_10bitCh(4);
	unsigned int AN2_voltage = a2d_10bitCh(3);
	unsigned int temp_currentTorqueDemand = AN1_voltage / 4;
	if (temp_currentTorqueDemand > 256) temp_currentTorqueDemand = 0;
	if (temp_currentTorqueDemand < 10) temp_currentTorqueDemand = 0;
	currentTorqueDemand[0] = temp_currentTorqueDemand;
	
	if(currentTorqueDemand[0] > 250) PORTA |= 32;
	else PORTA &= 223;
    
	itoa(currentTorqueDemand[0], tempBuffer, 10);
	uart1_puts(tempBuffer);
	uart1_puts("\r\n");
}

/**
*	Processes the data received from the CAN bus. Nominally will only be called from the CAN_pull_packet(..) function
*	in chassisCAN.h
**/
void CAN_process_data(uint8_t CANbus, uint8_t* data, uint8_t* numBytes, uint32_t* ID) {
	switch(CANbus) {
		case MCP2515_CAN1:
			if(inverters_save_data(data) == 0) {
				error_state(ERROR_INVERTER_RESPONSE);
			}
			break;
		case MCP2515_CAN2:
			break;
		case MCP2515_CAN3:
			break;
		default:
			break;
	}
}

/**
 * main()
 * Input:	none
 * Returns: 0 if the process finished with no errors
 * 
 * Initialises and maintain the ATmega microcontroller and the MCP2515 chip. 
 * Will idle the car until the ignition is started.
 * Then will proceed to act as the central brain in the electrical network of the car, collection and dispersing
 * data.
 **/
int main(void)
{
	_delay_ms(5);

	// Set up all devices to make the firmware actually run
	firmware_init();

	// Start the time that manages when to send heartbeats to the other devices in the CAN bus
	timer_init();

	// Set the function pointer that will be used to process data from the CAN bus
	CAN_process_data_ptr = &CAN_process_data;

	// Wait for the ignition pin to be pressed
	while((STATUS_REG & IGNITION) == 0)		
	{
		// send_heartbeat(PDM_H, NORMAL, 1);	
		led_toggle();			// This will slow flash the LED to indicate idle mode before ignition
		
		_delay_ms(1000);		
	}
	
	// Precharge the motor controllers, allow car start
	pdm_init();
	
	// Define the pedal positions.
	uint16_t throttle=0, brake=0, brakePressureF=0, brakePressureR=0;
	// Read the pedal values, error state if the thresholds are out of order.
	if(pedal_read(&brake, &throttle) == 0)throw_error_code(ERROR_PEDALS);	
	if(pressure_brake_read(&brakePressureF, &brakePressureR) == 0)throw_error_code(ERROR_BRAKE_PRESSURE);

	// 
	uint8_t steeringWheelData[8]={0,0,0,255,0,255,0,255};

	// Manage the car while it runs
    while (1) 
    {
		// Send the steering wheel data to the CAN bus
		MCP2515_wrapper_send(MCP2515_CAN3, 8,steeringWheelData,0x400000);
		steeringWheelData[1]++;	

		// Process data in the CAN bus if any have been received
		CAN_check_for_data();

		if((PINA & 128) == 128) inverterStatus = 0;
		else inverterStatus = 1;
		
		// 
		torque_calculate_current_demand();
		
		// Check if data has been received from UART and process it if it has
		if(isCharAvailable_1() == 1)uart_process_byte(receiveChar_1());
		
		//NEW STUFF, COMMENT OUT WHEN ADDING IT
		//read the pedal values, error state if the thresholds are out of order.
		if(pedal_read(&brake, &throttle) == 0)
            throw_error_code(ERROR_PEDALS);
		//read the pressure readings, checking for invalid values.
		if(pressure_brake_read(&brakePressureF, &brakePressureR) == 0)
            throw_error_code(ERROR_BRAKE_PRESSURE);
		
		steeringAngle = adc_read_avg(STEERING_ANGLE);

		uint16_t temp_ADC_read = a2d_10bitCh(1);

		int i = 0;
		while (radiator_cals_acewell_22k[i] > temp_ADC_read) i++;
		unsigned int j = ((radiator_cals_acewell_22k[i-1] - temp_ADC_read) * 30);
		temp_ADC_read = j / (radiator_cals_acewell_22k[i-1] - radiator_cals_acewell_22k[i]);
		T1_temp = temp_ADC_read + ((i * 3) + 25) * 10;
		
		// temp loop. if fan is off and temp is over x deg turn fan on
		// else if fan is on and temp is below y deg turn fan off
		if((T1_temp > 500) && ((pdm.flags[0] & 1) == 0))pdm.flags[0] |= 1;			//turn fan on
		else if ((T1_temp < 480) && ((pdm.flags[0] & 1) == 1)) pdm.flags[0] &= ~1;	//turn fan off
    }
	return 0;
}

// -------------------------------------------------- Interrupt Service Routines --------------------------------------------------

/**
 * INT1_vect
 * 
 * Change a bit in the STATUS_REG to show that CAN1 has data that needs to be processed
 **/
ISR(INT1_vect)	//CAN 1
{
	STATUS_REG |= CAN1_DataWaiting;
}

/**
 * INT0_vect
 * 
 * Change a bit in the STATUS_REG to show that CAN2 has data that needs to be processed
 **/
ISR(INT0_vect)	//CAN 2
{
	STATUS_REG |= CAN2_DataWaiting;
}

/**
 * PCINT0_vect
 * 
 * Change a bit in the STATUS_REG to show that CAN3 has data that needs to be processed
 **/
ISR(PCINT0_vect)//CAN 3
{
	STATUS_REG |= CAN3_DataWaiting;
}

/**
 * PCINT1_vect
 * 
 * Change a bit in the STATUS_REG to show that the ignition switch has been activated. This will allow the first loop in the 
 * main() function to proceed.
 **/
ISR(PCINT1_vect)		//ignition switch function
{
	if((IGNITION_PORT & (1<<IGNITION_PIN)) == 0)	//only enter further if the pin has gone low(active low)
	{
		if((STATUS_REG & IGNITION) == 0)	//only go further if we have not already ignited
		{
			for(uint8_t i = 0; i < 30; i++)	//loop 30 times in 0.1 second blocks checking that the pin is still low
			{
				_delay_ms(100);				//delay 100ms block
				led_toggle();
				if(IGNITION_PORT & (1<<IGNITION_PIN))	//check if the pin is still low
					break;
				if(i == 29)
				{
					STATUS_REG |= IGNITION;	//set ignition if its the last block of 100ms
					led_off();
				}
			}
		}
		else
		{
			for(uint8_t i = 0; i < 10; i++)	//loop 30 times in 0.1 second blocks checking that the pin is still low
			{
				_delay_ms(100);				//delay 100ms block
				led_toggle();
				if(IGNITION_PORT & (1<<IGNITION_PIN))	//check if the pin is still low
					break;
							
				if(i == 29)
				{
					STATUS_REG &= ~(IGNITION);	//set ignition if its the last block of 100ms
					led_off();
				}
			}
		}
		
	}
	led_off();	
}

/**
 * TIMER1_COMPA_vect
 * 
 * Send a CAN packet to the other devices periodically in order to maintain comm updates
 **/
ISR(TIMER1_COMPA_vect)
{
	heartbeatTimer++;
	if((heartbeatTimer%2) > 0)CAN_send_heartbeat(INVERTERS_ID, TORQUE_COMMAND, INVERTERS_ALL);
	
	switch(heartbeatTimer)
	{
		case 12:
			CAN_send_heartbeat(PDM_ID, NORMAL, 1);
			break;
		case 24:
			CAN_send_heartbeat(AMU_ID, NORMAL, ACCUMULATOR_FRONT);
			break;
		case 48:
			CAN_send_heartbeat(WHEEL_ID, NORMAL, 1);
			break;
		default:
			break;
	}
	
	if(heartbeatTimer > 48)heartbeatTimer = 0;		//500ms round robin for comms updates
}

