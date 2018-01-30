/*
 * ChassisController_V1.h
 *
 * Created: 5/12/2016 1:55:19 AM
 * Author : julius
 */ 
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "MCP2515_CC.h"
#define STATUS_REG GPIOR0
#define CAN1_DataWaiting 1
#define CAN2_DataWaiting 2
#define CAN3_DataWaiting 4

unsigned int avgVolts = 0;
unsigned int avgTemp = 0;
unsigned int minVolts = 0;
unsigned int minTemp = 0;
unsigned int maxVolts = 0;
unsigned int maxTemp = 0;