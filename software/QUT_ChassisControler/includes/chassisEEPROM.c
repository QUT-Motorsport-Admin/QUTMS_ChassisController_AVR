/**
 * @file chassisEEPROM.c
 * @author Joshua Copeland
 * @date 23/6/2018
 * @brief Contains functions for reading and writing data to and from the EEPROM storage
 **/

#include "chassisEEPROM.h";

// Function for reading from EEPROM, see header file for more info.
unsigned char eeprom_read(uint16_t address)
{
    // Wait for completion of previous write (wait for write enable bit to be 0).
    while(EECR & (1<<EEPE));

    // Set address register.
    EEAR = address;

    // Start eeprom read by setting read enable bit to 1.
    EECR |= (1<<EERE);

    // Return data from data register.
    return EEDR;
}

// Function for writing to EEPROM, see header file for more info.
void eeprom_write(uint16_t address, unsigned char data) {
    // Wait for completion of previous write (wait for write enable bit to be 0).
    while(EECR & (1<<EEPE));

    // Set address and data registers.
    EEAR = address;
    EEDR = data;

    // Set master write enable bit to 1.
    EECR |= (1<<EEMPE);

    // Start EEPROM write by setting write enable bit to 1.
    EECR |= (1<<EEPE);

    return 1;
}