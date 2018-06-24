/**
 * @file chassisPDM.c
 * @author Jonn Dillon
 * @date 16/4/2018
 * @brief Handles functions, calls and utilities related to the Power Distribution Board
 **/

#include "chassisPDM.h"

#include "chassisCAN.h"

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
	CAN_send_heartbeat(PDM_H, NORMAL, 1);
	_delay_ms(2000);
	pdm.flags[0] &= ~PDM_ATOMIC_ALARM;						// Stop sounding the RTD, we are ready to drive after one last transmission
	pdm.flags[0] |= PDM_SHUTDOWN_PLUS|PDM_SHUTDOWN_MINUS;	// Enable the relays to allow the starting of the car.
	pdm.flags[1] &= ~(PDM_PRECHARGE);						// Shutdown the precharge circuit after 2 seconds. should be more than enough time
	CAN_send_heartbeat(PDM_H, NORMAL, 1);
}