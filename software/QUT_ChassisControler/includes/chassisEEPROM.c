/**
 * @file chassisEEPROM.c
 * @author Joshua Copeland
 * @date 23/6/2018
 * @description Contains functions for reading and writing data to and from the EEPROM storage
 **/

/**
 * EEPROM Acronym Information
 * 
 * EEPROM registers, see section 8.6 of datasheet for more information.
 * EEAR: EEPROM Address Register
 * EEDR: EEPROM Data Register
 * EECR: EEPROM Control Register
 * 
 * EECR bits, see section 8.6.4 of datasheet for more information.
 * Bits 5-4 EEPM1 & EEPM0: EEPROM Programming Mode Bits
 * Bit 3 EERIE: EEPROM Ready Interrupt Enable
 * Bit 2 EEMPE: EEPROM Master Write Enable
 * Bit 1 EEPE: EEPROM Write Enable
 * Bit 0 EERE: EEPROM Read Enable
 **/

#include "chassisUART.h";

/**
 * See pages 42 & 43 of datasheet for example code this is based off.
 * 
 * Example Code:
 * 
 **/
unsigned char eeprom_read(uint8_t address)
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

/**
 * See pages 42 & 43 of datasheet for example code this is based off.
 * 
 * Example Code:
 * 
 **/
void eeprom_write(uint8_t address, unsigned char data) {
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