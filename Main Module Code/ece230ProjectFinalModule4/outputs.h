/*
 * outputs.h
 *
 *  Created on: Feb 18, 2025
 *      Author: youngcc
 */

#ifndef OUTPUTS_H_
#define OUTPUTS_H_


//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include "msp.h"
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#define LED_PORT P2
#define GAME_PORT P3
#define ACCEL_PORT P1

/* Constants */
#define NUM_LEDS 5 // variable for the number of LEDs for force-feedback
#define UPDATE_DELAY 50000 // timer A ticks between LED updates
#define HOLD_TIME 8 // time the LEDs stay lit

//// TODO change these array elements based on the number of LEDs you're using
//const uint8_t LED_PINS[NUM_LEDS] = {BIT3, BIT4, BIT5, BIT6, BIT7};
//const uint8_t ALL_LEDS = (BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
//const uint16_t LED_THRESHOLDS[NUM_LEDS] = {9000, 12000, 13500, 14250,14750};

// Define output pins for game input
#define GAME_OUT_0 BIT0  // P3.0
#define GAME_OUT_1 BIT2  // P3.2
#define GAME_OUT_2 BIT3  // P3.3
#define GAME_OUT_3 BIT5  // P3.5
#define GAME_OUT_4 BIT6  // P3.6

// Define acceleration ports
#define ACCEL_IN_LEFT  BIT5 // PIN 1.5
#define ACCEL_IN_RIGHT BIT6 // PIN 1.6


/*!
 * \brief This function configures led pins
 *
 * This function configures 2.3, 2.4, 2.5, 2.6, and 2.7
 * as GPIO outputs with no resistor enabled.
 *
 * \return None
 */
//extern void configure_led_pins(void);


/*!
 * \brief Configures game output pins
 *
 * This function configures pins 6.4, 6.5, 6.6, and 6.7 as GPIO
 * outputs
 *
 * \return None
 */
extern void game_output_init(void);

/*!
 * \brief This function gets the number of leds to light up
 *
 * This function uses the LED_THRESHOLDS array to determine how many
 * leds to light up based on the input adc_value that the ADC14
 * module will capture
 *
 * \return uint8_t
 */
//extern uint8_t get_led_count(uint16_t adc_value);

/*!
 * \brief Turns off all keypresses
 *
 * This function turns off all of the game ouput values by setting
 * them to 1. This will stop all keypresses.
 *
 * \return None
 */
extern void turn_off_game_output(void);

/*!
 * \brief Configures Timer A0 for lighting LEDs
 *
 * This function configures TimerA0 in up mode with SMCLK as the source,
 * a prescaler of 64, and interrupts enabled
 *
 * * \return None
 */
extern void TimerA0_Init(void);


/*!
 * \brief Configures GPIO Input pins
 *
 * This function configures the GPIO pins 1.5 and 1.6 to as inputs to accept
 * in processed acceleration booleans from the ESP32s
 *
 * \return None
 */
extern void Acceleration_Init(void);

#endif /* OUTPUTS_H_ */
