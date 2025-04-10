/*
 * FSRADC.c
 *
 * ADC configuration functions for the final project
 *
 *  Created on: Feb 17, 2025
 *      Author: youngcc
 */


#include "msp.h"
#include "FSRADC.h"

extern void ADC_pin_init(void) {

    // DONE Configure P5.5 for ADC (tertiary module function)
      Analog_PORT->DIR &= ~AnalogA0_PIN;
      Analog_PORT->SEL0 |= AnalogA0_PIN;
      Analog_PORT->SEL1 |= AnalogA0_PIN;

    // DONE Configure P5.4 for ADC (tertiary module function)
      Analog_PORT->DIR &= ~AnalogA1_PIN;
      Analog_PORT->SEL0 |= AnalogA1_PIN;
      Analog_PORT->SEL1 |= AnalogA1_PIN;

    // DONE Configure P5.2 for ADC (tertiary module function)
      Analog_PORT->DIR &= ~AnalogA3_PIN;
      Analog_PORT->SEL0 |= AnalogA3_PIN;
      Analog_PORT->SEL1 |= AnalogA3_PIN;

    // DONE Configure P5.1 for ADC (tertiary module function)
      Analog_PORT->DIR &= ~AnalogA4_PIN;
      Analog_PORT->SEL0 |= AnalogA4_PIN;
      Analog_PORT->SEL1 |= AnalogA4_PIN;

}

extern void ADC_memory_init(void) {

    ADC14->MCTL[0] = MEMORYCHANNEL_A0; // A0 P5.5
    ADC14->MCTL[1] = MEMORYCHANNEL_A1; // A1 P5.4
    ADC14->MCTL[2] = MEMORYCHANNEL_A3; // A3 P5.2
    ADC14->MCTL[3] = MEMORYCHANNEL_A4 | ADC14_MCTLN_EOS; // A4 P5.1 with EOS bit set

}



extern void ADC_control_init(void) {

    /* Configure ADC (CTL0 and CTL1) registers for:
         *      clock source - default MODCLK, clock prescale 1:1,
         *      sample input signal (SHI) source - software controlled (ADC14SC),
         *      Pulse Sample mode with sampling period of 16 ADC14CLK cycles,
         *      Sequence of channels, single-conversion, 14-bit resolution
         *      ADC14 conversion start address MEMORYCHANNEL_A0, and Low-power mode
         */
        ADC14->CTL0 = ADC14_CTL0_SHP                // Pulse Sample Mode
                        | ADC14_CTL0_SHT0__16       // 16 cycle sample-and-hold time (for ADC14MEM1) //* not sure what changing this does
                        | ADC14_CTL0_PDIV__1        // Predivide by 1
                        | ADC14_CTL0_DIV__1         // /1 clock divider
                        | ADC14_CTL0_SHS_0          // ADC14SC bit sample-and-hold source select
                        | ADC14_CTL0_SSEL__MODCLK   // clock source select MODCLK
                        | ADC14_CTL0_CONSEQ_1       //* Sequence of channels, single conversion
                        | ADC14_CTL0_ON;            // ADC14 on

        // DONE Configure ADC14MCTL1 and 0 as storage register for result
        //          Single-ended mode with Vref+ = Vcc and Vref- = Vss,
        //          Input channel - A1, and comparator window disabled
        //          Input channel - A0, and comparator window disabled
        //          Add an end of sequence bit on the second memory

        ADC14->CTL1 = ADC14_CTL1_RES__14BIT         //* change to 14-bit conversion results
                | (MEMORYCHANNEL_A0 << ADC14_CTL1_CSTARTADD_OFS) // ADC14MEM0 - conversion start address
                | ADC14_CTL1_PWRMD_0;               //* We changed this to normal power mode for 1 Msps

        // DONE Enable ADC conversion complete interrupt for ADC14MEM0 through ADC14MEM3
        ADC14->IER0 = ADC14_IER0_IE0 | ADC14_IER0_IE1 | ADC14_IER0_IE2 | ADC14_IER0_IE3;

        //DONE Enable conversion
        ADC14->CTL0 |= ADC14_CTL0_ENC;

        // Enable ADC interrupt in NVIC module
        NVIC->ISER[0] = (1 << ADC14_IRQn);

        // Enable global interrupt
        __enable_irq();

}


