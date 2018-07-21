/**
 * @file chassisUART.c
 * @author Jonn Dillon
 * @date 19/7/2018
 * @brief Handles all major communications functions, and related utilities in and out of the MCU.
 **/

#include "chassisUART.h"

// char *UART_getString(void) {
//     unsigned char string[255], x, i;
//     while((x = uart1_getc()) != UART_NO_DATA) {
//         string[i++] = x;
//     }
//     return string;
    
// }

// void UART_sendString(const char *s) {
//     uart1_puts(*s);
// }


// OLD ------------------------------------------------------------------------

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
	addr +=         (uint16_t)(s[2]);
	uint32_t data = ((uint32_t)s[3]) << 24;
	data =          ((uint32_t)s[4]) << 16;
	data +=         ((uint32_t)s[5]) << 8;
	data +=         ((uint32_t)s[6]);
	
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
			// cellBalancing = 1;
		}
		else
		{
			//turn LED off
			// cellBalancing = 0;
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
// void _uart_send_real_time_data (void)
// {
// 	uint8_t outgoingString[34];
// 	outgoingString[0] = 45;					//always start with capital D
// 	outgoingString[1] = 1;					//packet type (locked at 1 for now)
	
// 	outgoingString[2] = (accumulators[0].AvgV >> 8);
// 	outgoingString[3] = (accumulators[0].AvgV);	//average volts
	
// 	outgoingString[4] = (accumulators[0].AvgT >> 8);
// 	outgoingString[5] = (accumulators[0].AvgT);	//average temp

// 	outgoingString[6] = (accumulators[0].MinV >> 8);
// 	outgoingString[7] = (accumulators[0].MinV);	//average volts
	
// 	outgoingString[8] = (accumulators[0].MinT >> 8);
// 	outgoingString[9] = (accumulators[0].MinT);	//min temp
	
// 	outgoingString[10] = (accumulators[0].MaxV >> 8);
// 	outgoingString[11] = (accumulators[0].MaxV); 
	
// 	outgoingString[12] = (accumulators[0].MaxT >> 8);
// 	outgoingString[13] = (accumulators[0].MaxT);
	
// 	outgoingString[14] = (inverters[0].temperature >> 8);
// 	outgoingString[15] = (inverters[0].temperature);
	
// 	outgoingString[16] = (inverters[0].current >> 8);
// 	outgoingString[17] = (inverters[0].current);
	
// 	outgoingString[18] = (T1_temp >> 8); // Temp of right hand cooling ciruit
// 	outgoingString[19] = (T1_temp);
	
// 	outgoingString[20] = (pdm.flags[0]);
	
// 	outgoingString[21] = (accumulators[0].cmus[0].voltages[7]);
	
// 	outgoingString[22] = (currentTorqueDemand[0] >> 8);
// 	outgoingString[23] = (currentTorqueDemand[0]);	//demand given by the pedal position
	
// 	outgoingString[24] = (inverters[0].RPM >> 8);
// 	outgoingString[25] = (inverters[0].RPM);		//rpm of inverters
	
// 	outgoingString[26] = (enableSwitch_A);
// 	outgoingString[27] = (steeringAngle >> 8);
//     outgoingString[28] = (steeringAngle);
// 	outgoingString[29] = 128;
// 	outgoingString[30] = 128;
// 	outgoingString[31] = 128;
	
// 	for(int i = 0; i < 4; i++)
// 	{
// 		for(int j = 0; j < 7; j++)
// 		{
// 			if(outgoingString[(i*7) + j] == 68)			//test if the ith by jth byte is a 68
// 			{
// 				outgoingString[i + 28] |= (1 << j);		//set the corresponding bit in last 4 bytes to say this should be a 68
// 				outgoingString[(i*7) + j] = 0;			//any non 68 number really - doesnt matter
// 			}
// 		}
// 	}
	
// 	for (int i = 0; i < 32; i++) uart1_putc(outgoingString[i]);
// }

void uart_send_real_time_data (void)
{
	uint8_t outgoingString[34];
	outgoingString[0] = 45;					//always start with capital D
	outgoingString[1] = 1;					//packet type (locked at 1 for now)
	
	outgoingString[2] = 0;
	outgoingString[3] = 0;	//average volts
	
	outgoingString[4] = 0;
	outgoingString[5] = 0;	//average temp

	outgoingString[6] = 0;
	outgoingString[7] = 0;	//average volts
	
	outgoingString[8] = 0;
	outgoingString[9] = 0;	//min temp
	
	outgoingString[10] = 0;
	outgoingString[11] = 0; 
	
	outgoingString[12] = 0;
	outgoingString[13] = 0;
	
	outgoingString[14] = 0;
	outgoingString[15] = 0;
	
	outgoingString[16] = 0;
	outgoingString[17] = 0;
	
	outgoingString[18] = 0;
	outgoingString[19] = 0;
	
	outgoingString[20] = 0;
	
	outgoingString[21] = 0;
	
	outgoingString[22] = (INPUT_accelerationPedal >> 8);
	outgoingString[23] = INPUT_accelerationPedal;	//demand given by the pedal position
	
	outgoingString[24] = 0;
	outgoingString[25] = 0;		//rpm of inverters
	
	outgoingString[26] = 0;
	outgoingString[27] = 0;
    outgoingString[28] = 0;
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