/**
 * @file chassisEEPROM.h
 * @author Joshua Copeland
 * @date file creation 23/6/2018
 * @brief Functionality for reading and writing bytes to and from EEPROM
 **/

#ifndef CHASSIS_EEPROM_H
#define CHASSIS_EEPROM_H

#include <stdlib.h>
#include <avr/io.h>

/**
 * Chassis setting specification (which EEPROM addresses holds which settings)
 * Follows specification outlined in QUTMS_share/ChassisControl_firmware/Chassis_Settings_v1
 **/
// Sensor cals
#define EEPROM_UINT16_THROTTLE_PEDAL_RHS_0                  0x000
#define EEPROM_UINT16_THROTTLE_PEDAL_RHS_100                0x002
#define EEPROM_UINT16_THROTTLE_PEDAL_LHS_0                  0x004
#define EEPROM_UINT16_THROTTLE_PEDAL_LHS_100                0x006

#define EEPROM_UINT16_BRAKE_PEDAL_RHS_0                     0x008
#define EEPROM_UINT16_BRAKE_PEDAL_RHS_100                   0x00a
#define EEPROM_UINT16_BRAKE_PEDAL_LHS_0                     0x00c
#define EEPROM_UINT16_BRAKE_PEDAL_LHS_100                   0x00e

#define EEPROM_UINT16_STEERING_ANGLE_ZERO_POINT             0x010
#define EEPROM_UINT16_STEERING_ANGLE_RADIANS_PER_VOLT       0X012   // (x 1000)

#define EEPROM_UINT8_BRAKE_PRESSURE_FRONT_VOLTS_PER_BAR     0x020   // (x 100)
#define EEPROM_UINT8_BRAKE_PRESSURE_REAR_VOLTS_PER_BAR      0x021   // (x 100)

// Other settings
#define EEPROM_UINT8_REGEN_0_POINT                          0x030
#define EEPROM_UINT8_REGEN_RAMP_AMPS_PER_PERCENT            0x031
#define EEPROM_UINT8_REGEN_100_POINT                        0x032
#define EEPROM_UINT8_MAX_TORQUE_FRONT_INVERTERS             0x033
#define EEPROM_UINT8_MAX_TORQUE_REAR_INVERTERS              0x034

#define EEPROM_UINT8_MAX_REGEN_FRONT_INVERTERS              0x040
#define EEPROM_UINT8_MAX_REGEN_REAR_INVERTERS               0x041

#define EEPROM_UINT8_NUMBER_OF_ACCUMULATORS_FIRED           0x050
#define EEPROM_UINT8_NUMBER_OF_INVERTERS_FITTED             0x051

// Tables
#define EEPROM_UINT8_T64_INITIAL_TABLE                      0x100   // 0x100-0x13f
#define EEPROM_UINT8_T64_SUSTAINED_TABLE                    0x200   // 0x200-0x33f
#define EEPROM_UINT8_T64_FRONT_REAR_BIAS                    0x300   // 0x300-0x33f


/**
 * @function                  eeprom_read
 * @description               Reads a byte of data from the specified address of EEPROM storage
 * @author                    Joshua Copeland
 * @date_created              23/6/18
 * @date_modified             N/A
 * @param   uint16_t          address     - Address of EEPROM memory to read from
 * @returns unsigned char                 - Byte present in the target EEPROM address
 **/
unsigned char eeprom_read(uint16_t address);


/**
 * @function                 eeprom_write
 * @description              Writes a byte of data to the specified address of EEPROM storage
 * @author                   Joshua Copeland
 * @date_created             23/6/18
 * @date_modified            N/A
 * @param   uint8_t          address     - The address of EEPROM memory to write to
 * @param   unsigned char    data        - Byte to be written to the target EEPROM address
 * @returns void                         - Returns 1 when sucessful/completed
 **/
void eeprom_write(uint16_t address, unsigned char data);

#endif // CHASSIS_EEPROM_H