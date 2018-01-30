/*
 * ChassisController_V1.c
 *
 * Created: 5/12/2016 1:55:19 AM
 * Author : julius
 */ 
#include "ChassisController_V1.h"
#include "SPI.h"
#include "MCP2515_CC.h"
#include "uart.h"
#include "a2d.h"



void LED_flash(unsigned char times)
{
	for(uint8_t i = 0; i < times; i++)
	{
		PORTK ^= 0b00100000;
		_delay_ms(50);
		PORTK ^= 0b00100000;
		_delay_ms(50);
	}
}
void LED_toggle()
{
	PORTK ^= 0b00100000;
}
void LED_off()
{
	PORTK &= ~(1<<PINK5);
}

void ExternalInterrupt_init()
{
	//INT1 for CAN1, INT0 for CAN2, PCINT7 for CAN3
	EICRA |= (2<<ISC00)|(2<<ISC10);
	EIMSK = (1<<INT0) | (1<<INT1);
	//enable interrupts fot PCINT7:0
	PCICR = (1<<PCIE0)|(1<<PCIE1);
	PCMSK0 = (1<<PCINT7);
	PCMSK1 = (1<<PCINT15);
}

void IO_init()
{
	//pins 8, 9 and 14 for MCP2515_STB high so the things respond (PE6, PE7, PH2)
	DDRE = 0b11000010;		//PE0 = RS232 RX1; PE1 = RS232 TX1;  PE6 = STB_CAN1; PE7 = STB_CAN2;
	PORTE = 0b00000000;		
	DDRH = 0b00000111;		//PH0 = CS_CAN1; PH1 = CS_CAN2; PH2 = STB_CAN3
	PORTH = 0b00000011;		//CS_CAN1 high; CS_CAN2 high;
	//pins 12, 13, 19 for the CS for each MCP2515 PH0, PH1, PB0)

	//pin 21 for MOSI, pin 20  for SCK (PB2, PB1)
	DDRB = 0b01100111;		//PB0 = CS_CAN3; PB1 = SCK; PB2 = MOSI; PB3 = MISO; PB5 = High drive A; PB6 = Low drive A; PB7 = CAN3_INT; 
	PORTB = 0b00000001;		//set CS_CAN3 high;
	
	DDRL = 0b00011000;		//PB3 = High drive B; PB4 = Low Drive B;
	PORTL = 0b00000000;
	
	DDRD = 0b11001000;		//PD0 = CAN2_INT; PD1 = CAN1_INT; PD2 = RS232 RX2; PD3 = RS232 TX2; PD6 = CAN1_TXPIN; PD7 = CAN2_TXPIN;
	DDRD = 0b00000000;
	
	DDRC = 0b00001000;		//PC3 = CAN3_TXPIN;
	PORTC= 0b00000000;
	
	DDRJ = 0b00000000;		//PORTJ is used for digital input;
	
	DDRA = 0b00011000;		//PA3 = ENABLE_B; PA4 = ENABLE_A; PA1 = dig input; PA2 = dig input;
	PORTA |= (1<<PINA4);
	DDRK = 0b00100000;		//PK5 = debugging LED;
	PORTK = 0b00100000;
	
	ExternalInterrupt_init();
	//pin 26 is INT for CAN3
	
	//ping 43 and 44 for CAN2 and CAN1, PG2 and PA7
	/*DDRA  = 0b00110000;		//PA5 = LED2; PA4 = LED1
	DDRB  = 0b11110111;		//PB7 = INJ3; PB6 = INJ2; PB5 = INJ1; PB4 = AUX1; PB3 = MISO; PB2 = MOSI; PB1 = SCK; PB0 = SS
	DDRC  = 0b01000001;		//PC7 = SF_INJ12; PC6 = EN_INJ; PC1 = ST_AUX12; PC0 = CAN_SS
	DDRD  = 0b10001000;		//PD7 = CAN_TX0; PD5 = RTI2; PD4 = RTI1; PD3 = TX1; PD2 = RX1; PD1 = CAN_INT; PD0 = CAS2
	DDRE  = 0b00111010;		//PE7 = CAS1; PE6 = RTI3; PE5 = INJ4; PE4 = AUX4; PE3 = AUX3; PE2 = ST_AUX34; PE1 = TX0; PE0 = RX0
	DDRF  = 0b00000000;		//PORTF ALL ADCS
	DDRG  = 0b00111010;		//PG5 = AUX2; PG4 = Pulldown2; PG3 = Pulldown1; PG2 = ST_INJ34; PG1 = EN_AUX
		
	PORTA |= 0b11100000;	//turn on digital input pull-ups, turn LED2 on;
	PORTB |= 0b00011000;
	PORTC |= 0b00000001;	//set CAN_SS high

	PORTC |= 1;										//set CAN_CS high
		
	//interrupts
	EICRA = 0b00001000;		//enable INT1 on falling edges
	EIMSK = 0b00000010;		//enable INT1
	}*/
}
void Timer_init()
{
	TCCR1A = 0b00000000;			//CTC mode
	TCCR1B = 0b00001101;			//prescale clock by 1024
	OCR1A =  312;					//312 gives 50Hz main comms speed
	TIMSK1 = 0b00000010;			//turn on compare interrupt for OCR1A
}
void sendHeartbeat(unsigned char destination, unsigned char type, unsigned char address)
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
			MCP2515_TX(MCP2515_CAN1,mob, 8, (uint8_t*)currentTorqueDemand, ID);
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

uint16_t ADC_read_AVG(uint8_t ch)
{
	uint32_t adcSUM = 0;
	for(uint8_t i = 0; i<ADC_SAMPLES; i++)
	{
		adcSUM += a2d_10bitCh(ch);
	}
	return (uint16_t)(adcSUM/ADC_SAMPLES);
}

uint8_t Pressure_brake_read(uint16_t * front, uint16_t * rear)
{
	uint16_t tmp = 0;
	tmp = ADC_read_AVG(PRESSURE_BRAKE_FRONT);
	if(tmp < PRESSURE_BRAKE_LOW || tmp > PRESSURE_BRAKE_HIGH)return 0;
	*front = tmp;
	
	tmp = ADC_read_AVG(PRESSURE_BRAKE_REAR);
	if(tmp < PRESSURE_BRAKE_LOW || tmp > PRESSURE_BRAKE_HIGH)return 0;
	*rear = tmp;
	
	return 1;
		
}

uint8_t Pedal_read(uint16_t * brake, uint16_t * throttle)
{
	uint16_t primary = 0;
	uint16_t secondary = 0;
	int16_t delta = 0;
	
	//read with averaging, the two brake sensors, and check for invalid values.
	primary = ADC_read_AVG(PEDAL_BRAKE_CH1);
	if(primary < PEDAL_BRAKE_LOW || primary > PEDAL_BRAKE_HIGH)return 0;
	secondary = ADC_read_AVG(PEDAL_BRAKE_CH2);
	if(secondary < PEDAL_BRAKE_LOW || secondary > PEDAL_BRAKE_HIGH)return 0;
	//calculate difference between the two sensors
	delta = primary-secondary;
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
				sendHeartbeat(PDM_H, NORMAL, 1);
			}
		}
	}
	else return 0;
	
	//read with averaging, the throttle, and check for invalid values.
	primary = ADC_read_AVG(PEDAL_THROTTLE_CH1);
	if(primary < PEDAL_THROTTLE_LOW || primary > PEDAL_THROTTLE_HIGH)return 0;
	secondary = ADC_read_AVG(PEDAL_THROTTLE_CH2);
	if(secondary < PEDAL_THROTTLE_LOW || secondary > PEDAL_THROTTLE_HIGH)return 0;
	//calculate difference between the two sensors
	delta = abs(primary-secondary);
	if(delta > PEDAL_DELTA_THRESH_L && delta < PEDAL_DELTA_THRESH_H)
	{
		*throttle = primary;
	}
	else return 0;
	
	return 1;
}

void UART_processByte(char data)
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
		UART_parseInput(incomingString);
	}
}

void UART_parseInput(unsigned char* s)
{
	UART_parsePoke(s);
	uart1_putc('D');		//reply with the header byte (preserved - 33 bytes should follow)
	UART_sendRealTimeData();

	
	s[0] = '\0';				// clear the header byte
	
}

void UART_parsePoke(unsigned char* s)
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

void UART_sendRealTimeData (void)
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

void error_state(uint16_t error)
{
	//do error reporting here
}

void shutdown_probe()
{
	if(STOP_BRAKE_OVERTRAVEL)shutdown_state(SHDN_BRAKE_OVERTRAVEL);
	if(STOP_DRIVER_ESTOP)shutdown_state(SHDN_DRIVER_ESTOP);
	if(STOP_INERTIA_SWITCH)shutdown_state(SHDN_INERTIA_SWITCH);
	if(STOP_LEFT_FRONT_UPRIGHT)shutdown_state(SHDN_LEFT_FRONT_UPRIGHT);
	if(STOP_RIGHT_FRONT_UPRIGHT)shutdown_state(SHDN_RIGHT_FRONT_UPRIGHT);
}

void shutdown_state(uint16_t shutdownFlag)
{
	//do shutdown reporting here
}

uint8_t CAN1_Process(uint8_t numBytes, uint8_t * data, uint32_t ID)
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

uint8_t CAN2_Process(uint8_t numBytes, uint8_t * data, uint32_t ID)
{
	if(ID & HEARTBEAT_PDM_ID)
	{
		
	}
	else if(ID & HEARTBEAT_AMU_ID)
	{
	
	}
	return 0;
}

void CAN3_Process()
{
	
}

int main(void)
{
	_delay_ms(5);
	IO_init();
	SPI_init();
	uart1_init(19200);
	a2dInit(ADC_PRESCALE_DIV64, ADC_REFERENCE_AVCC); //Turns ON also
	MCP2515_init(MCP2515_CAN1);
	MCP2515_init(MCP2515_CAN2);
	MCP2515_init(MCP2515_CAN3);
	
	sei();
	PORTJ |= (1<<PINJ6);		//enable the pullup on the input. This allows the pin to be active low
	Timer_init();
	while((STATUS_REG & IGNITION) == 0)		//wait for the ignition pin to be pressed
	{
		//sendHeartbeat(PDM_H, NORMAL, 1);	
		LED_toggle();			//this will slow flash the LED to indicate idle mode before ignition
		
		
		_delay_ms(1000);		
		
	}
	
	pdm.flags[0] |= PDM_ATOMIC_ALARM;	//turn on the ready to drive alarm (RTD sound)
	pdm.flags[1] |= PDM_PRECHARGE;		//begin precharging the Motor controllers
	sendHeartbeat(PDM_H, NORMAL, 1);
	_delay_ms(2000);
	pdm.flags[0] &= ~PDM_ATOMIC_ALARM;		//stop sounding the RTD, we are ready to drive after one last transmission
	pdm.flags[0] |= PDM_SHUTDOWN_PLUS|PDM_SHUTDOWN_MINUS;	//enable the relays to allow the starting of the car.
	pdm.flags[1] &= ~(PDM_PRECHARGE);		//shutdown the precharge circuit after 2 seconds. should be more than enough time
	sendHeartbeat(PDM_H, NORMAL, 1);
	
	//initialise inverter structs
	for(uint8_t i = 0; i < NUM_INVERTERS; i++)
	{
		inverters[i].ID=1<<i;
		inverters[i].current = 0;
		inverters[i].duty = 0;
		inverters[i].RPM = 0;
		inverters[i].temperature = 0;
	}

	accumulators[0].ID=ACCUMULATOR_FRONT;
	
	//define the pedal positions.
	uint16_t throttle=0, brake=0, brakePressureF=0, brakePressureR=0;
	//read the pedal values, error state if the thresholds are out of order.
	if(Pedal_read(&throttle, &brake) == 0)error_state(ERROR_PEDALS);	//read the initial state of the pedal. if it is
	if(Pressure_brake_read(&brakePressureF, &brakePressureR) == 0)error_state(ERROR_BRAKE_PRESSURE);

	uint8_t steeringWheelData[8]={0,0,0,255,0,255,0,255};
    while (1) 
    {

		MCP2515_TX(MCP2515_CAN3, MCP2515_findFreeTxBuffer(MCP2515_CAN3),8,steeringWheelData,0x400000);
		steeringWheelData[1]++;
		
		
		//check for available TX buffer
		unsigned int AN1_voltage = a2d_10bitCh(4);
		unsigned int AN2_voltage = a2d_10bitCh(3);
		
		
		if(STATUS_REG & CAN1_DataWaiting)
		{
			uint8_t status = MCP2515_check_receive_status(MCP2515_CAN1);
			uint8_t data[8];
			uint32_t ID;
			uint8_t numBytes;
			switch(status>>6)
			{
				case 1:
					//pull the data off the MCP2515 and place in memory
					MCP2515_PullCanPacket(MCP2515_CAN1, MCP2515_RXB0SIDH, &numBytes, data, &ID);
					if(CAN1_Process(numBytes, data, ID) == 0)error_state(ERROR_INVERTER_RESPONSE);
					break;
				case 2:
					MCP2515_PullCanPacket(MCP2515_CAN1, MCP2515_RXB1SIDH, &numBytes, data, &ID );
					if(CAN1_Process(numBytes, data, ID) == 0)error_state(ERROR_INVERTER_RESPONSE);
					break;
				case 3:
					MCP2515_PullCanPacket(MCP2515_CAN1, MCP2515_RXB0SIDH, &numBytes, data, &ID );
					if(CAN1_Process(numBytes, data, ID) == 0)error_state(ERROR_INVERTER_RESPONSE);
					MCP2515_PullCanPacket(MCP2515_CAN1, MCP2515_RXB1SIDH, &numBytes, data, &ID );
					if(CAN1_Process(numBytes, data, ID) == 0)error_state(ERROR_INVERTER_RESPONSE);
					break;
				default:
					break;
			}
			STATUS_REG &= ~(CAN1_DataWaiting);
		}
		
		
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
					if(CAN2_Process(numBytes, data, ID) == 0)error_state(ERROR_CAN2_RESPONSE);
					break;
				case 2:
					MCP2515_PullCanPacket(MCP2515_CAN2, MCP2515_RXB1SIDH, &numBytes, data, &ID);
					if(CAN2_Process(numBytes, data, ID) == 0)error_state(ERROR_CAN2_RESPONSE);
					break;
				case 3:
					MCP2515_PullCanPacket(MCP2515_CAN2, MCP2515_RXB0SIDH, &numBytes, data, &ID);
					if(CAN2_Process(numBytes, data, ID) == 0)error_state(ERROR_CAN2_RESPONSE);
					MCP2515_PullCanPacket(MCP2515_CAN2, MCP2515_RXB1SIDH, &numBytes, data, &ID);
					if(CAN2_Process(numBytes, data, ID) == 0)error_state(ERROR_CAN2_RESPONSE);
					break;
			}
		}
		if(STATUS_REG & CAN3_DataWaiting)
		{
			
		}
		
		
		
		
		if((PINA & 128) == 128) inverterStatus = 0;
		else inverterStatus = 1;
		
		unsigned int temp_currentTorqueDemand = AN1_voltage / 4;
		if (temp_currentTorqueDemand > 256) temp_currentTorqueDemand = 0;
		if (temp_currentTorqueDemand < 10) temp_currentTorqueDemand = 0;
		currentTorqueDemand[0] = temp_currentTorqueDemand;
		
		if(currentTorqueDemand[0] > 250) PORTA |= 32;
		else PORTA &= 223;
		
		//CANreceiver = MCP2515_receive_status();
		itoa(currentTorqueDemand[0], tempBuffer, 10);
		uart1_puts(tempBuffer);
		uart1_puts("\r\n");
		
		if(isCharAvailable_1() == 1)UART_processByte(receiveChar_1());
		
		//NEW STUFF, COMMENT OUT WHEN ADDING IT
		//read the pedal values, error state if the thresholds are out of order.
		if(Pedal_read(&brake, &throttle) == 0)error_state(ERROR_PEDALS);
		//read the pressure readings, checking for invalid values.
		if(Pressure_brake_read(&brakePressureF, &brakePressureR) == 0)error_state(ERROR_BRAKE_PRESSURE);
		
		steeringAngle = ADC_read_AVG(STEERING_ANGLE);
		
		
		
		uint16_t temp_ADC_read = a2d_10bitCh(1);
		int i = 0;
		
		while (radiator_cals_acewell_22k[i] > temp_ADC_read) i++;
		unsigned int j = ((radiator_cals_acewell_22k[i-1] - temp_ADC_read) * 30);
		temp_ADC_read = j / (radiator_cals_acewell_22k[i-1] - radiator_cals_acewell_22k[i]);
		T1_temp = temp_ADC_read + ((i * 3) + 25) * 10;
		
		// temp loop. if fan is off and temp is over x deg turn fan on
		// else if fan is on and temp is blow y deg turn fan off
		if((T1_temp > 500) && ((pdm.flags[0] & 1) == 0))pdm.flags[0] |= 1;			//turn fan on
		else if ((T1_temp < 480) && ((pdm.flags[0] & 1) == 1)) pdm.flags[0] &= ~1;	//turn fan off
    }
	return 0;
}

ISR(INT1_vect)	//CAN 1
{
	STATUS_REG |= CAN1_DataWaiting;
}

ISR(INT0_vect)	//CAN 2
{
	STATUS_REG |= CAN2_DataWaiting;
}

ISR(PCINT0_vect)//CAN 3
{
	STATUS_REG |= CAN3_DataWaiting;
}

ISR(PCINT1_vect)		//ignition switch function
{
	
	if((IGNITION_PORT & (1<<IGNITION_PIN)) == 0)	//only enter further if the pin has gone low(active low)
	{
		if((STATUS_REG & IGNITION) == 0)	//only go further if we have not already ignited
		{
			for(uint8_t i = 0; i < 30; i++)	//loop 30 times in 0.1 second blocks checking that the pin is still low
			{
				_delay_ms(100);				//delay 100ms block
				LED_toggle();
				if(IGNITION_PORT & (1<<IGNITION_PIN))	//check if the pin is still low
					break;
					
				if(i == 29)
				{
					STATUS_REG |= IGNITION;	//set ignition if its the last block of 100ms
					LED_off();
				}
			}
		}
		else
		{
			for(uint8_t i = 0; i < 10; i++)	//loop 30 times in 0.1 second blocks checking that the pin is still low
			{
				_delay_ms(100);				//delay 100ms block
				LED_toggle();
				if(IGNITION_PORT & (1<<IGNITION_PIN))	//check if the pin is still low
					break;
							
				if(i == 29)
				{
					STATUS_REG &= ~(IGNITION);	//set ignition if its the last block of 100ms
					LED_off();
				}
			}
		}
		
	}
	LED_off();	
}

ISR(TIMER1_COMPA_vect)
{
	//sendHeartbeat(INVERTERS);			//send can packets to devices at the prescribed intervals
	heartbeatTimer++;
	//LED_toggle();
	if((heartbeatTimer%2) > 0)sendHeartbeat(INVERTERS, TORQUE_COMMAND, INVERTERS_ALL);
	
	switch(heartbeatTimer)
	{
		case 12:
			sendHeartbeat(PDM_H, NORMAL, 1);
			break;
		case 24:
			sendHeartbeat(AMU_H, NORMAL, ACCUMULATOR_FRONT);
			break;

		case 48:
			sendHeartbeat(WHEEL, NORMAL, 1);
			break;
		default:
			break;
	}
	
	if(heartbeatTimer > 48)heartbeatTimer = 0;		//500ms round robin for comms updates
	
	
	//	sendHeartbeat();			//send can packets to devices at the prescribed intervals
}

