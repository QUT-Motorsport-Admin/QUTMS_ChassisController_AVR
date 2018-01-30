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
#include <stdlib.h>

#define NUM_INVERTERS	4
#define NUM_ACCUMULATORS	1
#define NUM_CMU_PER_AMU	6
#define CMU_CELL_COUNT	8


#define STATUS_REG GPIOR0
#define CAN1_DataWaiting	1
#define CAN2_DataWaiting	2
#define CAN3_DataWaiting	4
#define IGNITION			8

#define HEARTBEAT_INV_ID	0x0C000000
#define	HEARTBEAT_AMU_ID	0x0A000000
#define HEARTBEAT_PDM_ID	0x09000000
#define HEARTBEAT_WHEEL_ID	0x00400000

#define PEDAL_BRAKE_HIGH	900	//temp value
#define PEDAL_BRAKE_LOW		100	//temp value
#define PEDAL_BRAKE_LIGHT_ON 512 //temp value, must update with testing

#define PEDAL_THROTTLE_HIGH	900	//temp value
#define PEDAL_THROTTLE_LOW	100	//temp value
#define PEDAL_DELTA_THRESH_L	50
#define PEDAL_DELTA_THRESH_H	150

//ADC channel allocation
#define PEDAL_BRAKE_CH1		10	
#define PEDAL_BRAKE_CH2		8
#define PEDAL_THROTTLE_CH1	5
#define PEDAL_THROTTLE_CH2	2
#define RADIATOR_RIGHT		0	//AN5
#define RADIATOR_LEFT		12	//AN10
#define PRESSURE_BRAKE_FRONT	11	//AN6
#define PRESSURE_BRAKE_REAR		7	//AN7
#define STEERING_ANGLE			4//AN8
//single sensor, no error checking

#define PRESSURE_BRAKE_HIGH		1022
#define PRESSURE_BRAKE_LOW		1


#define ADC_SAMPLES	10

#define ERROR_PEDALS 1
#define ERROR_BRAKE_PRESSURE 2
#define ERROR_INVERTER_RESPONSE 3
#define ERROR_CAN2_RESPONSE	4


#define PDM_BRAKELIGHT 16				//byte 0
#define PDM_ATOMIC_ALARM	32			//byte 0
#define PDM_SHUTDOWN_PLUS	64			//byte 0
#define PDM_SHUTDOWN_MINUS	128			//byte 0
#define PDM_PRECHARGE		1			//byte 1
//shutdown circuits
//seg 4 pin 63 dig 1 pinj0	//inertia switch
//seg 5 pin 64 dig 2 pinj1	//left front upright
//seg 6 pin 66 dig 3 pinj3	//brake overtravel
//seg 7 pin 68 dig 4 pinj5	//right front upright
//seg 8 pin 77 dig 5 pina1	//driver E-Stop

//digital inputs
//dig 6 pin 90	//driver switch

//digital outputs
//pin 25	//driver lamp a
//pin 24	//driver lamp b

#define STOP_INERTIA_SWITCH			PINJ&(1<<PINJ0)
#define STOP_LEFT_FRONT_UPRIGHT		PINJ&(1<<PINJ1)
#define STOP_BRAKE_OVERTRAVEL		PINJ&(1<<PINJ3)
#define STOP_RIGHT_FRONT_UPRIGHT	PINJ&(1<<PINJ5)
#define STOP_DRIVER_ESTOP			PINA&(1<<PINA1)

#define SHDN_INERTIA_SWITCH			1
#define SHDN_LEFT_FRONT_UPRIGHT		2
#define SHDN_BRAKE_OVERTRAVEL		4
#define SHDN_RIGHT_FRONT_UPRIGHT	8
#define SHDN_DRIVER_ESTOP			16
#define IGNITION_PORT				PINJ
#define IGNITION_PIN				PJ6


unsigned int avgVolts = 0;
unsigned int avgTemp = 0;
unsigned int minVolts = 0;
unsigned int minTemp = 0;
unsigned int maxVolts = 0;
unsigned int maxTemp = 0;

int testValue1;
int testValue2;

unsigned int temps[0];
unsigned int voltages[8];

unsigned char enableSwitch_A = 0;

unsigned int MC_oilTemp[4] = {0,0,0,0};
unsigned char PDM_data[4] = {2, 0, 0, 0};

unsigned char cellBalancing = 0;
unsigned int AN1_voltage = 0;
unsigned int AN2_voltage = 0;
unsigned int currentTorqueDemand[4] = {0, 0, 0, 0};
unsigned char inverterStatus = 0;
unsigned int currentRPM = 0;
unsigned int inverterTemp = 0;
unsigned int inverterDCcurrent = 0;
uint16_t	 steeringAngle = 0;
unsigned char operatingMode = 0;

unsigned int T1_temp = 0;
typedef struct Inverter
{
	uint8_t ID;
	uint16_t RPM;
	uint16_t temperature;
	uint16_t current;
	uint16_t duty;
}Inverter;

typedef struct CMU
{
	uint8_t CMU_num, CMU_ID;
	uint16_t voltages[CMU_CELL_COUNT];	//will change with change of parameter. Unlikely to.
	uint16_t temperatures[CMU_CELL_COUNT];
}CMU;

typedef struct Accumulator
{
	uint8_t flags[4];
	uint8_t ID;
	uint16_t MinV;
	uint16_t MaxV;
	uint16_t AvgV;
	uint16_t AvgT;
	uint16_t MinT;
	uint16_t MaxT;
	CMU cmus[NUM_CMU_PER_AMU];
}Accumulator;

typedef struct PDM
{
	uint8_t flags[4];

}PDM;

typedef struct STEERINGWHEEL
{
	uint8_t flags[4];

}STEERINGWHEEL;

struct STEERINGWHEEL steeringWheel;
struct PDM	pdm;
struct Accumulator	accumulators[NUM_ACCUMULATORS];
struct Inverter inverters[NUM_INVERTERS];

unsigned int testTimer = 0;
unsigned char CANreceiver = 0;
unsigned int radiator_cals_acewell_22k[27] = {801,	800, 799, 797, 791, 785, 767, 750, 734, 707, 689, 671, 637, 598, 581, 562, 529,
493, 464, 443, 359, 338, 317, 297, 278, 234, 204};
volatile int heartbeatTimer = 0;
char CANdiagnostics[10][20];
uint8_t tempBuffer[10];

#define CANselect PORTC &= ~1
#define CANdeselect	  PORTC |= 1

void flash_LED(unsigned char times);

#define INVERTERS 1
#define PDM_H 2
#define AMU_H	3
#define WHEEL 4
#define INVERTERS_ALL 0b1111
#define TORQUE_COMMAND 3
#define NORMAL 1
#define ACCUMULATOR_FRONT 1
#define ACCUMULATOR_REAR 2


void shutdown_state(uint16_t shutdownFlag);
void UART_sendRealTimeData (void);
void UART_parseInput(unsigned char* s);
void UART_processByte(char data);
void UART_parsePoke(unsigned char* s);
void CAN2ascii(uint8_t CANdata,	char * CANresult);