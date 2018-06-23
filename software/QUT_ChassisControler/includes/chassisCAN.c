/**
 * @file chassisShutDown
 * @author Jonn Dillon
 * @date 16/4/2018
 * @description Handles functions, calls and utilities related to the CANBUS's
 **/

#include "chassisCAN.h";
#include "../utils/MCP2515.c";


void MCP2515_wrapper_send(uint8_t CANbus, uint8_t numBytes, uint8_t * data, uint32_t ID) {
    MCP2515_TX(CANbus, MCP2515_findFreeTxBuffer(CANbus), numBytes, data, ID);
}

uint32_t MCP2515_wrapper_ID_constructor(uint32_t sendingID, unsigned char type, unsigned char address, uint32_t status) {
    return (
        sendingID|                  // Sending ID, ( First 8 bits, define the device to send to)
        ((uint32_t)type<<18)|       // What sort of command to send
        ((uint32_t)address<<13)|    // Specific address
        status                      // SubID?
    );
}



// OLD ------------------------------------------------------------------------

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
	uint8_t freeTxBuffer;
	uint32_t ID = 0;
	switch(destination)
	{
		case INVERTERS_ID:
			//obtain a freeTxBuffer that is free
			freeTxBuffer = MCP2515_findFreeTxBuffer(MCP2515_CAN1);
			//type = what sort of command. address is which inverters should listen, status is whether the inverters are active or not.
			// ID = (HEARTBEAT_INV_ID|((uint32_t)type<<18)|((uint32_t)address<<13)|inverterStatus);
            ID = MCP2515_wrapper_ID_constructor(CAN_ID_INV, type, address, inverterStatus);
			//transmit the packet.
			MCP2515_TX(MCP2515_CAN1, freeTxBuffer, 8, (uint8_t*)currentTorqueDemand, ID);
			break;
		case PDM_ID:
			//unsigned char TXstatus = MCP2515_reg_read(MCP2515_CAN2, 0x30);		//check the tx reg status
			//obtain a freeTxBuffer that is free
			freeTxBuffer = MCP2515_findFreeTxBuffer(MCP2515_CAN2);
			//type = what sort of command. address is which address of pdm, normal heartbeat packet;
			// ID = (HEARTBEAT_PDM_ID|((uint32_t)type<<18)|((uint32_t)address<<13)|1);
            ID = MCP2515_wrapper_ID_constructor(CAN_ID_PDM, type, address, 1);
			//transmit the packet.
			pdm.flags[0]=10;
			MCP2515_TX(MCP2515_CAN2, freeTxBuffer, 4, pdm.flags, ID);
			break;
		case AMU_ID:
			//obtain a freeTxBuffer that is free
			freeTxBuffer = MCP2515_findFreeTxBuffer(MCP2515_CAN2);
			//type = what sort of command. address is which address of AMU, normal heartbeat packet;
			// ID = (HEARTBEAT_AMU_ID|((uint32_t)type<<18)|((uint32_t)address<<13)|1);
            ID = MCP2515_wrapper_ID_constructor(CAN_ID_AMU, type, address, 1);
			//transmit the packet.
			MCP2515_TX(MCP2515_CAN2, freeTxBuffer, 4, accumulators[0].flags, ID);
			break;
		case WHEEL_ID:
			//obtain a freeTxBuffer that is free
			freeTxBuffer = MCP2515_findFreeTxBuffer(MCP2515_CAN3);
			//type = what sort of command. address is which address of AMU, normal heartbeat packet;
			// ID = (HEARTBEAT_WHEEL_ID|((uint32_t)type<<18)|((uint32_t)address<<13)|1);
            ID = MCP2515_wrapper_ID_constructor(CAN_ID_WHEEL, type, address, 1);
			MCP2515_TX(MCP2515_CAN3, freeTxBuffer, 4, steeringWheel.flags, ID);
			break;
		default:
			break;
		
	}
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