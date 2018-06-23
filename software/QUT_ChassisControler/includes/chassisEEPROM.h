/**
 * @file chassisEEPROM
 * @author Joshua Copeland
 * @date file creation 23/6/2018
 * @description Functionality for reading and writing bytes to and from EEPROM
 **/

#ifndef CHASSIS_EEPROM_H
#define CHASSIS_EEPROM_H

#include <stdlib.h>
#include <avr/io.h>
#include <../main.h>

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