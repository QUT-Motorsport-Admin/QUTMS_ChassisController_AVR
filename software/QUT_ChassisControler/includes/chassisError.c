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