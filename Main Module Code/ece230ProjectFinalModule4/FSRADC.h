/*
 * FSRADC.h
 *
 *  Description: header file for the FSR ADC conversion functions and associated
 *  FSRADC.c file.
 *
 *  Created on: Feb 17, 2025
 *      Author: youngcc
 */

#ifndef FSRADC_H_
#define FSRADC_H_

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

#include "msp.h"
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#define Analog_PORT P5
#define AnalogA0_PIN BIT5 // analog A0 is on pin 5.5
#define MEMORYCHANNEL_A0 0x0

#define AnalogA1_PIN BIT4    // analog A1 is on pin 5.4
#define MEMORYCHANNEL_A1 0x1

#define AnalogA3_PIN BIT2 // analog A3 is on pin 5.2
#define MEMORYCHANNEL_A3 0x3 //

#define AnalogA4_PIN BIT1 // analog A4 is on pin 5.1
#define MEMORYCHANNEL_A4 0x4

/*!
 * \brief This function configures the ADC14 pins
 *
 * This function configures pins 5.5, 5.4, 5.2, and 5.1
 * as ADC14 inputs corresponding to A0, A1, A3, and A4
 * respectively.
 *
 * \return None
 */
extern void ADC_pin_init(void);


/*!
 * \brief This function configures the ADC14 memory channels
 *
 * This function configures the ADC14 memory channels in order
 * corresponding to the inputs 5.5/A0, 5.4/A1, 5.2/A3, and 5.3/A4
 *
 */
extern void ADC_memory_init(void);

/*!
 * \brief This function configures the ADC14 control word
 *
 * This function configures the ADC14 control word for
 *
     * clock source - default MODCLK, clock prescale 1:1,
     *      sample input signal (SHI) source - software controlled (ADC14SC),
     *      Pulse Sample mode with sampling period of 16 ADC14CLK cycles,
     *      Sequence of channels, single-conversion, 14-bit resolution
     *      ADC14 conversion start address MEMORYCHANNEL_A0, and Low-power mode
     *
 * and
 *
    *     Single-ended mode with Vref+ = Vcc and Vref- = Vss,
    *     Input channel - A1, and comparator window disabled
    *     Input channel - A0, and comparator window disabled
    *     Add an end of sequence bit on the second memory
 *
 * It ALSO configures the Interrupts! As in it enables interrupts for the 4 memory
 * channels that the ADC uses in this project and also enables global interrupts.
 * \return None
 *
 */
extern void ADC_control_init(void);


#endif /* FSRADC_H_ */
