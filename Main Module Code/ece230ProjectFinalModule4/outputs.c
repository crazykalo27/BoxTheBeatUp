/*
 * outputs.c
 *
 * Configuration files for LED and game outputs as well as some functionality
 * for calculating which led or game output to turn on. Also includes
 * timer configuration for the leds.
 *
 *  Created on: Feb 18, 2025
 *      Author: youngcc
 */


#include "outputs.h"
#include "msp.h"

//extern void configure_led_pins(void) {
//    LED_PORT->SEL0 &= ~(ALL_LEDS);  // Select GPIO
//    LED_PORT->SEL1 &= ~(ALL_LEDS);
//    LED_PORT->DIR |= (ALL_LEDS);    // Output
//    LED_PORT->OUT &= ~(ALL_LEDS);   // Turn off initially
//}


//extern uint8_t get_led_count(uint16_t adc_value) {
//    uint8_t num_lit = 0;
//    int i = 0;
//    for(i = 0; i < NUM_LEDS; i++){
//        if(adc_value > LED_THRESHOLDS[i]){
//            num_lit++;
//        } else {
//            break; // efficiency
//        }
//    }
//    return num_lit;
//}


void game_output_init(void) {
    GAME_PORT->SEL0 &= ~(GAME_OUT_0 | GAME_OUT_1 | GAME_OUT_2 | GAME_OUT_3 | GAME_OUT_4); // select GPIO function
    GAME_PORT->SEL1 &= ~(GAME_OUT_0 | GAME_OUT_1 | GAME_OUT_2 | GAME_OUT_3 | GAME_OUT_4);
    GAME_PORT->DIR |= (GAME_OUT_0 | GAME_OUT_1 | GAME_OUT_2 | GAME_OUT_3 | GAME_OUT_4);  // Set as outputs
    GAME_PORT->OUT |= (GAME_OUT_0 | GAME_OUT_1 | GAME_OUT_2 | GAME_OUT_3 | GAME_OUT_4);  // Default HIGH
}

void turn_off_game_output(void) {
    GAME_PORT->OUT |= (GAME_OUT_0 | GAME_OUT_1 | GAME_OUT_2 | GAME_OUT_3  | GAME_OUT_4);  // Turn them off (active low)
}


extern void TimerA0_Init(void) {
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 |  // SMCLK source
                    TIMER_A_CTL_MC_1 |      // up mode
                    TIMER_A_CTL_ID_3 |      // predivide by 8
                    TIMER_A_CTL_CLR;        // clear

    TIMER_A0->CCR[0] = UPDATE_DELAY; // set period to change the LED update speed
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // enable timer A0 interrupt

    TIMER_A0->EX0 |= TIMER_A_EX0_TAIDEX_7; // predivide by another 8 for a total of 64

    NVIC_EnableIRQ(TA0_0_IRQn); // enable interrupt in NVIC

}


extern void Acceleration_Init(void) {
    ACCEL_PORT->SEL0 &= ~(ACCEL_IN_LEFT | ACCEL_IN_RIGHT); // select GPIO function
    ACCEL_PORT->SEL1 &= ~(ACCEL_IN_LEFT | ACCEL_IN_RIGHT);
    ACCEL_PORT->DIR &= ~(ACCEL_IN_LEFT | ACCEL_IN_RIGHT);  // Set as inputs
    ACCEL_PORT->REN &= ~(ACCEL_IN_LEFT | ACCEL_IN_RIGHT);  // Disable pull-up/down resistors
}
