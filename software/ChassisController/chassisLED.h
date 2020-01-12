/**
 * @file chassisLED.h
 * @author Jonn Dillon
 * @date 08/07/2018
 * @brief Defines and outlines all variables and definitions related to the onboard LED
 **/

#ifndef CHASSIS_LED_H
#define CHASSIS_LED_H

#include <stdlib.h>
#include <avr/io.h>

/**
 * @brief Toggles the LED state
 */
void led_toggle(void);

/**
 * @brief Turns off the LED
 */
void led_off(void);

/**
 * @brief Turns on the LED
 */
void led_on(void);


#endif // CHASSIS_LED_H