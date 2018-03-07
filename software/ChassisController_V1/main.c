// ===================================================================
// ChassisController_V1.c
//
// Author : julius
// Created: 5/12/2016 1:55:19 AM
// Last Altered: 
// ===================================================================

#include "ChassisController_V1.h"
#include "SPI.h"
#include "MCP2515.h"
#include "uart.h"
#include "a2d.h"

/**
 * led_flash()
 * Input:	times - The amount of times to make the LED blink
 * Returns: none
 * 
 * Makes the LED blink a set number of times.
 * The delay between the state of the LED (on/off) is 50ms. The period of the blinks is 100ms
 * 
 * Reference: ATmega Datasheet Chapter 13 (I/O-Ports)
 **/
void led_flash(unsigned char times)
{
	for(uint8_t i = 0; i < times; i++)
	{
		PORTK ^= 1<<PINK5;
		_delay_ms(50);
		PORTK ^= 1<<PINK5;
		_delay_ms(50);
	}
}

/**
 * led_toggle()
 * Input:	none
 * Returns: none
 * 
 * Switches between the two statues of the LED (on -> off or off -> on)
 * 
 * Reference: ATmega Datasheet Chapter 13 (I/O-Ports)
 **/
void led_toggle()
{
	PORTK ^= 1<<PINK5;
}

/**
 * led_off()
 * Input:	none
 * Returns: none
 * 
 * Turns off the LED
 * 
 * Reference: ATmega Datasheet Chapter 13 (I/O-Ports)
 **/
void led_off()
{
	PORTK &= ~(1<<PINK5);
}

/**
 * external_interrupt_init()
 * Input:	none
 * Returns: none
 * 
 * Sets up the microcontroller to allow external interrupts. The External Interrupts are triggered by the INT7:0 pin or any of the PCINT23:0 pins.
 * 
 * Reference: ATmega Datasheet Chapter 15 (External Interrupts)
 **/
void external_interrupt_init()
{
	//INT1 for CAN1, INT0 for CAN2, PCINT7 for CAN3
	EICRA |= (2<<ISC00)|(2<<ISC10);
	EIMSK  = (1<<INT0) | (1<<INT1);
	//Enable interrupts for PCINT7:0
	PCICR  = (1<<PCIE0)|(1<<PCIE1);
	PCMSK0 = (1<<PCINT7);
	PCMSK1 = (1<<PCINT15);
}

/**
 * io_init()
 * Input:	none
 * Returns: none
 * 
 * Configures the pins required for IO. Each port pin consists of three register bits: DDxn, PORTxn, and PINxn set by the following registers
 * 	DDRx   - Sets the direction of PINxn in bit DDxn (1 -> output, 0 -> input)
 * 	PORTx  - Activates the pull-up resistor for PINxn in bit PORTxn. The pull-up resistor will only be activated if written as logic 1 AND set as an input
 *  PINx   - The Port Input Pins I/O location is read only. 
 *           Writing a logic one to a bit in the PINx Register, will result in a toggle in the corresponding bit in the Data Register
 * 		     Writing a logic one to PINxn toggles the value of PORTxn, independent on the value of DDRxn
 * 
 * Reference: ATmega Datasheet Chapter 13 (I/O-Ports)
 **/
void io_init()
{
	//pins 8, 9 and 14 for MCP2515_STB high so the things respond (PE6, PE7, PH2)
	DDRE  = 0b11000010;		//PE0 = RS232 RX1; PE1 = RS232 TX1;  PE6 = STB_CAN1; PE7 = STB_CAN2;
	PORTE = 0b00000000;		
	DDRH  = 0b00000111;		//PH0 = CS_CAN1; PH1 = CS_CAN2; PH2 = STB_CAN3
	PORTH = 0b00000011;		//CS_CAN1 high; CS_CAN2 high;
	//pins 12, 13, 19 for the CS for each MCP2515 PH0, PH1, PB0)

	//pin 21 for MOSI, pin 20  for SCK (PB2, PB1)
	DDRB  = 0b01100111;		//PB0 = CS_CAN3; PB1 = SCK; PB2 = MOSI; PB3 = MISO; PB5 = High drive A; PB6 = Low drive A; PB7 = CAN3_INT; 
	PORTB = 0b00000001;		//set CS_CAN3 high;
	
	DDRL  = 0b00011000;		//PB3 = High drive B; PB4 = Low Drive B;
	PORTL = 0b00000000;
	
	DDRD  = 0b11001000;		//PD0 = CAN2_INT; PD1 = CAN1_INT; PD2 = RS232 RX2; PD3 = RS232 TX2; PD6 = CAN1_TXPIN; PD7 = CAN2_TXPIN;
	DDRD  = 0b00000000;
	
	DDRC  = 0b00001000;		//PC3 = CAN3_TXPIN;
	PORTC = 0b00000000;
	
	DDRJ  = 0b00000000;		//PORTJ is used for digital input;
	
	DDRA  = 0b00011000;		//PA3 = ENABLE_B; PA4 = ENABLE_A; PA1 = dig input; PA2 = dig input;
	PORTA = 0b00010000;
	DDRK  = 0b00100000;		//PK5 = debugging LED;
	PORTK = 0b00100000;
	
	// Enable external interrupts in order for the CAN bus to communicate with us
	external_interrupt_init();
}

/**
 * timer_init()
 * Input:	none
 * Returns: none
 * 
 * Initiates a timer set on Clear Timer Compare Match (CTC) Mode.
 * 
 * Reference: ATmega Datasheet Chapter 17 (16-bit Timer/Counter)
 **/
void timer_init()
{
	TCCR1A = 0b00000000;			//CTC mode
	TCCR1B = 0b00001101;			//prescale clock by 1024
	OCR1A =  312;					//312 gives 50Hz main comms speed
	TIMSK1 = 0b00000010;			//turn on compare interrupt for OCR1A
}

/**
 * send_heartbeat()
 * Input:	destination	-	The device that will receive the packet	
 * 			type		-	What sort of command to send
 * 			address		-	The specific address in the device to transmit to
 * Returns: none
 * 
 * Send a CAN packet to a specific device in the CAN bus in order to maintain comm updates
 * 
 * Reference: MCP2515 Datasheet Chapter 3 (Message Transmission)
 **/
void send_heartbeat(unsigned char destination, unsigned char type, unsigned char address)
{	
	uint8_t mob;
	uint32_t ID = 0;
	switch(destination)
	{
		case INVERTERS:
			//obtain a mob that is free
			mob = MCP2515_findFreeTxBuffer(MCP2515_CAN1);
			//type = what sort of command. address is which inverters should listen, status is whether the inverters are active or not.
			ID = (HEARTBEAT_INV_ID|((uint32_t)type<<18)|((uint32_t)address<<13)|inverterStatus);	
			//transmit the packet.
			MCP2515_TX(MCP2515_CAN1, mob, 8, (uint8_t*)currentTorqueDemand, ID);
			break;
		case PDM_H:
			//unsigned char TXstatus = MCP2515_reg_read(MCP2515_CAN2, 0x30);		//check the tx reg status
			//obtain a mob that is free
			mob = MCP2515_findFreeTxBuffer(MCP2515_CAN2);
			//type = what sort of command. address is which address of pdm, normal heartbeat packet;
			ID = (HEARTBEAT_PDM_ID|((uint32_t)type<<18)|((uint32_t)address<<13)|1);
			//transmit the packet.
			pdm.flags[0]=10;
			MCP2515_TX(MCP2515_CAN2,mob, 4, pdm.flags, ID);
			break;
		case AMU_H:
			//obtain a mob that is free
			mob = MCP2515_findFreeTxBuffer(MCP2515_CAN2);
			//type = what sort of command. address is which address of AMU, normal heartbeat packet;
			ID = (HEARTBEAT_AMU_ID|((uint32_t)type<<18)|((uint32_t)address<<13)|1);
			//transmit the packet.
			MCP2515_TX(MCP2515_CAN2,mob, 4, accumulators[0].flags, ID);
			break;
		case WHEEL:
			//obtain a mob that is free
			mob = MCP2515_findFreeTxBuffer(MCP2515_CAN3);
			//type = what sort of command. address is which address of AMU, normal heartbeat packet;
			ID = (HEARTBEAT_WHEEL_ID|((uint32_t)type<<18)|((uint32_t)address<<13)|1);
			MCP2515_TX(MCP2515_CAN3,mob, 4, steeringWheel.flags, ID);
			break;
		default:
			break;
		
	}
}

/**
 * adc_read_avg()
 * Input:	ch	-	The analog input channel
 * Returns: 10 bit digital value representative of the analog input in the specified channel
 * 
 * The inbuilt ADC is used to convert an analog input voltage at the specified channel to a 10-bit digital 
 * value through successive approximation.
 * This is done <ADC_SAMPLES> times and the results are averaged in order to obtain a more accurate value.
 * 
 * Reference: ATmega Datasheet Chapter 26 (ADC - Analog to Digital Converter)
 **/
uint16_t adc_read_avg(uint8_t ch)
{
	uint32_t adcSUM = 0;
	for(uint8_t i = 0; i<ADC_SAMPLES; i++)
	{
		adcSUM += a2d_10bitCh(ch);
	}
	return (uint16_t)(adcSUM/ADC_SAMPLES);
}

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

/**
 * uart_process_byte()
 * Input:	data	-	a byte incoming from UART
 * Returns: none
 * 
 * Makes sure the byte is either a header for the UART packet or that a header has already been passed, making the
 * message valid.
 * Adds the byte to memory where the message is being stored.
 **/
void uart_process_byte(char data)
{
	//PORTA ^= 32;
	
	static unsigned char count = 0;
	static unsigned char incomingString[48];							//made bigger to accommodate PT packets
	static unsigned char stringActive = 0;
	
	if(data == 'D')
	{
		stringActive = 1;
	}
	
	else if(stringActive == 1)
	{
		incomingString[count++] = data;
	}
	
	if(count > 8)
	{
		incomingString[count - 1] = '\0';
		count = 0;
		stringActive = 0;
		uart_parse_input(incomingString);
	}
}

/**
 * uart_parse_input()
 * Input:	s	-	a string containing the message to be parsed
 * Returns: none
 * 
 * 
 **/
void uart_parse_input(unsigned char* s)
{
	uart_parse_poke(s);
	uart1_putc('D');		//reply with the header byte (preserved - 33 bytes should follow)
	uart_send_real_time_data();

	s[0] = '\0';				// clear the header byte
	
}

/**
 * uart_parse_poke()
 * Input:	s	- a string containing the message to be parsed
 * Returns: none
 * 
 * 
 **/
void uart_parse_poke(unsigned char* s)
{
	uint16_t addr = ((uint16_t)(s[1]) << 8);
	addr += (uint16_t)(s[2]);
	uint32_t data = ((uint32_t)s[3]) << 24;
	data = ((uint32_t)s[4]) << 16;
	data += ((uint32_t)s[5]) << 8;
	data += ((uint32_t)s[6]);
	
	if (addr == 20)
	{
		if (data == 128)
		{
			
		}
		else
		{
			
		}
	}
	
	else if (addr == 21)
	{
		if (data == 128)
		{
			
		}
		else
		{
			
		}
	}
	
	else if (addr == 200)		//balancing command
	{
		PORTA ^= 32;
		if (data == 1)
		{
			//turn LED on
			cellBalancing = 1;
		}
		else
		{
			//turn LED off
			cellBalancing = 0;
		}
	}
}

/**
 * uart_send_real_time_data()
 * Input:	none
 * Returns: none
 * 
 * Organizes all data stored in memory regarding the other devices into a message to be sent via UART
 **/
void uart_send_real_time_data (void)
{
	uint8_t outgoingString[34];
	outgoingString[0] = 45;					//always start with capital D
	outgoingString[1] = 1;					//packet type (locked at 1 for now)
	
	outgoingString[2] = (accumulators[0].AvgV >> 8);
	outgoingString[3] = (accumulators[0].AvgV);	//average volts
	
	outgoingString[4] = (accumulators[0].AvgT >> 8);
	outgoingString[5] = (accumulators[0].AvgT);	//average temp

	outgoingString[6] = (accumulators[0].MinV >> 8);
	outgoingString[7] = (accumulators[0].AvgV);	//average volts
	
	outgoingString[8] = (accumulators[0].MinT >> 8);
	outgoingString[9] = (accumulators[0].MinT);	//min temp
	
	outgoingString[10] = (accumulators[0].MaxV >> 8);
	outgoingString[11] = (accumulators[0].MaxV); 
	
	outgoingString[12] = (accumulators[0].MaxT >> 8);
	outgoingString[13] = (accumulators[0].MaxT);
	
	outgoingString[14] = (inverters[0].temperature >> 8);
	outgoingString[15] = (inverters[0].temperature);
	
	outgoingString[16] = (inverters[0].current >> 8);
	outgoingString[17] = (inverters[0].current);
	
	outgoingString[18] = (T1_temp >> 8);
	outgoingString[19] = (T1_temp);
	
	outgoingString[20] = (pdm.flags[0]);
	
	outgoingString[21] = (accumulators[0].cmus[0].voltages[7]);
	
	outgoingString[22] = (currentTorqueDemand[0] >> 8);
	outgoingString[23] = (currentTorqueDemand[0]);	//demand given by the pedal position
	
	outgoingString[24] = (inverters[0].RPM >> 8);
	outgoingString[25] = (inverters[0].RPM);		//rpm of inverters
	
	outgoingString[26] = (enableSwitch_A);
	outgoingString[28] = (steeringAngle >> 8);
	outgoingString[29] = 128;
	outgoingString[30] = 128;
	outgoingString[31] = 128;
	
	
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 7; j++)
		{
			if(outgoingString[(i*7) + j] == 68)			//test if the ith by jth byte is a 68
			{
				outgoingString[i + 28] |= (1 << j);		//set the corresponding bit in last 4 bytes to say this should be a 68
				outgoingString[(i*7) + j] = 0;			//any non 68 number really - doesnt matter
			}
		}
	}
	
	
	for (int i = 0; i < 32; i++) uart1_putc(outgoingString[i]);
}

/**
 * error_state()
 * Input:	error	-
 * Returns: none
 * 
 * Not implemented yet
 **/
void error_state(uint16_t error)
{
	//do error reporting here
}

/**
 * shutdown_probe()
 * Input:	none
 * Returns: none
 * 
 * Checks if a shutdown switch has been activated. Sends the method of shutdown to be recorded
 * 
 * Reference: ATmega Datasheet Chapter 13 (I/O-Ports)
 **/
void shutdown_probe()
{
	if(STOP_BRAKE_OVERTRAVEL)shutdown_state(SHDN_BRAKE_OVERTRAVEL);
	if(STOP_DRIVER_ESTOP)shutdown_state(SHDN_DRIVER_ESTOP);
	if(STOP_INERTIA_SWITCH)shutdown_state(SHDN_INERTIA_SWITCH);
	if(STOP_LEFT_FRONT_UPRIGHT)shutdown_state(SHDN_LEFT_FRONT_UPRIGHT);
	if(STOP_RIGHT_FRONT_UPRIGHT)shutdown_state(SHDN_RIGHT_FRONT_UPRIGHT);
}

/**
 * shutdown_state()
 * Input:	shutdownFlag	-
 * Returns: none
 * 
 * Not implemented yet
 **/
void shutdown_state(uint16_t shutdownFlag)
{
	//do shutdown reporting here
}

/**
 * inverters_save_data()
 * Input:	data	-	The data received from the inverters in the CAN bus
 * Returns: 0 if there was something wrong with the CAN packet resulting in no inverter being found from the ID given. 1 if execution is nominal
 * 
 * Process the data that was received from the inverters and saves it to the proper variables
 **/
uint8_t inverters_save_data(uint8_t data)
{
	//loop through all inverters
	for(uint8_t i = 0; i < NUM_INVERTERS; i++)
	{
		//if the ID contains 0b0001, 0b0010, 0b0100 or 0b1000, then assign the data from the can packet to this inverter.
		if(((ID>>13)&0b1111) & 1<<i)
		{
			inverters[i].RPM = data[1];
			inverters[i].RPM |= data[0]<<8;
			inverters[i].temperature = data[3];
			inverters[i].temperature |= data[2]<<8;
			inverters[i].current = data[4];
			inverters[i].current |= data[5]<<8;
			inverters[i].duty = data[7];
			inverters[i].duty |= data[6];
			return 1;
		}
	}
	return 0;
}

/**
 * can1_process()
 * Input:	none
 * Returns: none
 * 
 * Checks if have received any data from the other devices in the CAN1 bus then proceeds to pull data off the 
 * CAN bus to be processed and saved to memory
 * 
 * Reference: MCP2515 Datasheet Chapter 12 (SPI Interface)
 **/
uint8_t can1_process()
{
	if(STATUS_REG & CAN1_DataWaiting)
		{
			uint8_t status = MCP2515_check_receive_status(MCP2515_CAN1);
			uint8_t data[8];
			uint32_t ID;
			uint8_t numBytes;
			// Check which receive buffer contains the data (or if both contain) by checking bits 7:6
			switch(status>>6)
			{
				// Message in RXB0
				case 1:
					//pull the data off the MCP2515 and place in memory
					MCP2515_PullCanPacket(MCP2515_CAN1, MCP2515_RXB0SIDH, &numBytes, data, &ID);
					if(inverters_save_data(data) == 0)error_state(ERROR_INVERTER_RESPONSE);
					break;
				// Message in RXB1
				case 2:
					MCP2515_PullCanPacket(MCP2515_CAN1, MCP2515_RXB1SIDH, &numBytes, data, &ID );
					if(inverters_save_data(data) == 0)error_state(ERROR_INVERTER_RESPONSE);
					break;
				// Message in both buffers
				case 3:
					MCP2515_PullCanPacket(MCP2515_CAN1, MCP2515_RXB0SIDH, &numBytes, data, &ID );
					if(inverters_save_data(data) == 0)error_state(ERROR_INVERTER_RESPONSE);
					MCP2515_PullCanPacket(MCP2515_CAN1, MCP2515_RXB1SIDH, &numBytes, data, &ID );
					if(inverters_save_data(data) == 0)error_state(ERROR_INVERTER_RESPONSE);
					break;
				default:
					break;
			}
			STATUS_REG &= ~(CAN1_DataWaiting);
		}
}

/**
 * can2_save_data()
 * Input:	data	-	The data received from the CAN2 bus
 * Returns: 0 if there was something wrong with the CAN packet resulting in no IDs being matched. 1 if execution is nominal
 * 
 * Not implemented yet
 * 
 * Process the data that was received fom CAN2 
 **/
uint8_t can2_save_data(uint32_t ID, uint8_t data)
{
	if(ID & HEARTBEAT_PDM_ID)
	{
		
	}
	else if(ID & HEARTBEAT_AMU_ID)
	{
	
	}
	return 0;
}

/**
 * can2_process()
 * Input:	none
 * Returns: none
 * 
 * Checks if have received any data from the other devices in the CAN2 bus then proceeds to pull data off the 
 * CAN bus to be processed and saved to memory
 * 
 * Not fully implemented yet. Data is pulled from CAN2 bus but nothing is done with it
 * 
 * Reference: MCP2515 Datasheet Chapter 12 (SPI Interface)
 **/
void can2_process()
{
	if(STATUS_REG & CAN2_DataWaiting)
	{
		uint8_t status = MCP2515_check_receive_status(MCP2515_CAN2);
		uint8_t data[8];
		uint32_t ID;
		uint8_t numBytes;
		switch(status>>6)
		{
			case 1:
				MCP2515_PullCanPacket(MCP2515_CAN2, MCP2515_RXB0SIDH, &numBytes, data, &ID);
				if(can2_save_data(data, ID) == 0)error_state(ERROR_CAN2_RESPONSE);
				break;
			case 2:
				MCP2515_PullCanPacket(MCP2515_CAN2, MCP2515_RXB1SIDH, &numBytes, data, &ID);
				if(can2_save_data(data, ID) == 0)error_state(ERROR_CAN2_RESPONSE);
				break;
			case 3:
				MCP2515_PullCanPacket(MCP2515_CAN2, MCP2515_RXB0SIDH, &numBytes, data, &ID);
				if(can2_save_data(data, ID) == 0)error_state(ERROR_CAN2_RESPONSE);
				MCP2515_PullCanPacket(MCP2515_CAN2, MCP2515_RXB1SIDH, &numBytes, data, &ID);
				if(can2_save_data(data, ID) == 0)error_state(ERROR_CAN2_RESPONSE);
				break;
		}
	}

}

/**
 * can3_process()
 * Input:	none
 * Returns: none
 * 
 * Checks if have received any data from the other devices in the CAN3 bus then proceeds to pull data off the 
 * CAN bus to be processed and saved to memory
 * 
 * Not implemented yet
 * 
 * Reference: MCP2515 Datasheet Chapter 12 (SPI Interface)
 **/
void can3_process()
{
	if(STATUS_REG & CAN3_DataWaiting)
	{
		
	}
}

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
 * pdm_init()
 * Input:	none
 * Returns: none
 * 
 * Precharge the motor controllers and then enable relays to allow the starting of the car
 **/
void pdm_init() 
{
	pdm.flags[0] |= PDM_ATOMIC_ALARM;						// Turn on the ready to drive alarm (RTD sound)
	pdm.flags[1] |= PDM_PRECHARGE;							// Begin precharging the Motor controllers
	send_heartbeat(PDM_H, NORMAL, 1);
	_delay_ms(2000);
	pdm.flags[0] &= ~PDM_ATOMIC_ALARM;						// Stop sounding the RTD, we are ready to drive after one last transmission
	pdm.flags[0] |= PDM_SHUTDOWN_PLUS|PDM_SHUTDOWN_MINUS;	// Enable the relays to allow the starting of the car.
	pdm.flags[1] &= ~(PDM_PRECHARGE);						// Shutdown the precharge circuit after 2 seconds. should be more than enough time
	send_heartbeat(PDM_H, NORMAL, 1);
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

	io_init();
	SPI_init();
	uart1_init(19200);
	a2dInit(ADC_PRESCALE_DIV64, ADC_REFERENCE_AVCC); // Turns ON also
	MCP2515_init(MCP2515_CAN1);
	MCP2515_init(MCP2515_CAN2);
	MCP2515_init(MCP2515_CAN3);
	
	sei();

	// Enable the pullup on the input. This allows the pin to be active low
	PORTJ |= (1<<PINJ6);

	timer_init();

	// Wait for the ignition pin to be pressed
	while((STATUS_REG & IGNITION) == 0)		
	{
		//send_heartbeat(PDM_H, NORMAL, 1);	
		led_toggle();			// This will slow flash the LED to indicate idle mode before ignition
		
		_delay_ms(1000);		
	}
	
	pdm_init();
	
	// Initialise inverter structs
	for(uint8_t i = 0; i < NUM_INVERTERS; i++)
	{
		inverters[i].ID=1<<i;
		inverters[i].current = 0;
		inverters[i].duty = 0;
		inverters[i].RPM = 0;
		inverters[i].temperature = 0;
	}

	accumulators[0].ID=ACCUMULATOR_FRONT;
	
	// Define the pedal positions.
	uint16_t throttle=0, brake=0, brakePressureF=0, brakePressureR=0;

	// Read the pedal values, error state if the thresholds are out of order.
	if(pedal_read(&brake, &throttle) == 0)error_state(ERROR_PEDALS);	
	if(pressure_brake_read(&brakePressureF, &brakePressureR) == 0)error_state(ERROR_BRAKE_PRESSURE);

	uint8_t steeringWheelData[8]={0,0,0,255,0,255,0,255};

	// Manage the car while it runs
    while (1) 
    {
		// Send the steering wheel data to the CAN bus
		MCP2515_TX(MCP2515_CAN3, MCP2515_findFreeTxBuffer(MCP2515_CAN3),8,steeringWheelData,0x400000);
		steeringWheelData[1]++;	

		// Process data in the CAN bus if any have been received
		can1_process();
		can2_process();
		can3_process();

		if((PINA & 128) == 128) inverterStatus = 0;
		else inverterStatus = 1;
		
		torque_calculate_current_demand();
		
		if(isCharAvailable_1() == 1)uart_process_byte(receiveChar_1());
		
		//NEW STUFF, COMMENT OUT WHEN ADDING IT
		//read the pedal values, error state if the thresholds are out of order.
		if(pedal_read(&brake, &throttle) == 0)error_state(ERROR_PEDALS);
		//read the pressure readings, checking for invalid values.
		if(pressure_brake_read(&brakePressureF, &brakePressureR) == 0)error_state(ERROR_BRAKE_PRESSURE);
		
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
 * main() function to prooceed.
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
	if((heartbeatTimer%2) > 0)send_heartbeat(INVERTERS, TORQUE_COMMAND, INVERTERS_ALL);
	
	switch(heartbeatTimer)
	{
		case 12:
			send_heartbeat(PDM_H, NORMAL, 1);
			break;
		case 24:
			send_heartbeat(AMU_H, NORMAL, ACCUMULATOR_FRONT);
			break;
		case 48:
			send_heartbeat(WHEEL, NORMAL, 1);
			break;
		default:
			break;
	}
	
	if(heartbeatTimer > 48)heartbeatTimer = 0;		//500ms round robin for comms updates
}

