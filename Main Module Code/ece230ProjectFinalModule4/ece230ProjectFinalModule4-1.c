//ece230FinalProjectModule3-1.c
//ECE230 Winter 2024-2025
//Authored by Colin Young
/************************************************************
 * Four Channel ADC reader for voltage dividers
 *
 * Description: Analog inputs are on A0, A1, A3, A4. Manually triggered
 * analog-to-digital conversions with interrupt on-completion.
 * * Author:    Colin Young
 * Last-modified:   2/21/2025
 * *                 MSP432P411x
 *             -------------------
 *         /|\|                   |
 *          | |                   |
 *          --|RST        P5.5/A0 |<--- FSR Voltage Divider
 *            |                   |
 *  P2.3 <--- | LED0      P5.4/A1 |<--- FSR Voltage Divider
 *  P2.4 <--- | LED1              |
 *  P2.5 <--- | LED2      P5.2/A3 |<--- FSR Voltage Divider
 *  P2.6 <--- | LED3              |
 *  P2.7 <--- | LED4      P5.1/A4 |<--- FSR Voltage Divider
 *            |                   |
 *            |                   |---> Console output
 *
***********************************************************/
#include "msp.h"
#include "FSRADC.h"
#include "csHFXT.h"
#include "outputs.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

/* Constants */
#define NUM_LEDS 5 // variable for the number of LEDs for force-feedback
#define UPDATE_DELAY 50000 // timer A ticks between LED updates
#define HOLD_TIME 8 // time the LEDs stay lit

// TODO change these array elements based on the number of LEDs you're using
const uint8_t LED_PINS[NUM_LEDS] = {BIT3, BIT4, BIT5, BIT6, BIT7};
const uint8_t ALL_LEDS = (BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
const uint16_t LED_THRESHOLDS[NUM_LEDS] = {9000, 12000, 13500, 14250,14750};

/* Globals */
volatile uint8_t led_count = 0;  // Global variable to store how many LEDs light up
volatile uint8_t start_led_effect = 0; // Flag to trigger LED animation in TIMERA0 ISR
volatile uint8_t current_led = 0; // Track which LED is lighting up
volatile uint8_t turning_off = 0; // Flag to indicate turn-off sequence
volatile uint8_t hold_time = HOLD_TIME; // hold time before turning off LEDs
volatile uint8_t countdown = 0; // countdown for turning off the keycode
volatile uint8_t glove_punching = 0; // left glove (P1.5) is zero and right (P1.6) glove is one

/* Statics */
static volatile uint16_t curADC0Result;
static volatile uint16_t curADC1Result;
static volatile uint16_t curADC3Result;
static volatile uint16_t curADC4Result;
static volatile bool resultReady = false;

void configure_led_pins() {
    LED_PORT->SEL0 &= ~(ALL_LEDS);  // Select GPIO
    LED_PORT->SEL1 &= ~(ALL_LEDS);
    LED_PORT->DIR |= (ALL_LEDS);    // Output
    LED_PORT->OUT &= ~(ALL_LEDS);   // Turn off initially
}

uint8_t get_led_count(uint16_t adc_value) {
    uint8_t num_lit = 0;
    int i = 0;
    for(i = 0; i < NUM_LEDS; i++){
        if(adc_value > LED_THRESHOLDS[i]){
            num_lit++;
        } else {
            break; // efficiency
        }
    }
    return num_lit;
}


void main(void)
{

//    float AnalogVoltage0, AnalogVoltage1, AnalogVoltage3, AnalogVoltage4; // store both analog voltages
//
//    float MaxVoltage0 = 0, MaxVoltage1 = 0, MaxVoltage3 = 0, MaxVoltage4 = 0; // max voltages for printing
//    uint8_t PrintCount;

    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize the LED pins
    configure_led_pins();

    // initialize the timer and clock
    configHFXT();
    TimerA0_Init();

    // initialize the ADC14 register for FSR voltage acquisition
    ADC_pin_init();
    ADC_memory_init();
    ADC_control_init();

    // initialize the game output pins
    game_output_init();

    // initialize the acceleration inputs
    Acceleration_Init();

    while (1)
    {
        // start the sampling/conversion
        ADC14->CTL0 |= ADC14_CTL0_SC;

        // wait for conversion to complete
        while (!resultReady);

//        AnalogVoltage0 = (float) curADC0Result*3.3/pow(2,14);
//        AnalogVoltage1 = (float) curADC1Result*3.3/pow(2,14);
//        AnalogVoltage3 = (float) curADC3Result*3.3/pow(2,14);
//        AnalogVoltage4 = (float) curADC4Result*3.3/pow(2,14);
//
//
//        // this statement changes the max voltage
//        if(AnalogVoltage0 > MaxVoltage0){MaxVoltage0 = AnalogVoltage0;}
//        if(AnalogVoltage1 > MaxVoltage1){MaxVoltage1 = AnalogVoltage1;}
//        if(AnalogVoltage3 > MaxVoltage3){MaxVoltage3 = AnalogVoltage3;}
//        if(AnalogVoltage4 > MaxVoltage4){MaxVoltage4 = AnalogVoltage4;}
//
//
//         print the max and current values
//        PrintCount++;
//        if(PrintCount > 10){
//            printf("\r\nM0 = %5.3f    M1 = %5.3f    M2 = %5.3f    M3 = %5.3f", AnalogVoltage0, AnalogVoltage1, AnalogVoltage3, AnalogVoltage4);
//            PrintCount = 0;
//        }

        uint8_t LR_reading = (ACCEL_PORT->IN & (ACCEL_IN_LEFT | ACCEL_IN_RIGHT));
        if(LR_reading != 0) { glove_punching = LR_reading >> 6; }


        resultReady = false;
    }
}


// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {

    // Check if interrupt triggered by ADC14MEM0 conversion value loaded
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG0) {

        uint8_t led_count_MEM0;

        curADC0Result = ADC14->MEM[0]; // store the analog voltage value
        ADC14->CTL0 |= ADC14_CTL0_SC; // start the next conversion in the sequence
        led_count_MEM0 = get_led_count(curADC0Result); // set the number of LEDs based on force
        if(led_count_MEM0 > 0) {
            start_led_effect = 1; // set the flag to trigger LED effect in timer ISR
            led_count = led_count_MEM0;
            if(glove_punching == 0) {
                GAME_PORT->OUT &= ~(GAME_OUT_0);
            } else {
                GAME_PORT->OUT &= ~(GAME_OUT_1);
            }
            countdown = 2;
        }

    } // not necessary to clear flag because reading ADC14MEMx clears flag

    if (ADC14->IFGR0 & ADC14_IFGR0_IFG1) {

        uint8_t led_count_MEM1;

        curADC1Result = ADC14->MEM[1]; // store the analog voltage value
        ADC14->CTL0 |= ADC14_CTL0_SC; // start the next conversion in the sequence
        led_count_MEM1 = get_led_count(curADC1Result); // set the number of LEDs based on force
        if(led_count_MEM1 > 0) {
            start_led_effect = 1; // set the flag to trigger LED effect in timer ISR
            led_count = led_count_MEM1;
            GAME_PORT->OUT &= ~(GAME_OUT_2);
            countdown = 2;
        }
    }

    if (ADC14->IFGR0 & ADC14_IFGR0_IFG2) {

        uint8_t led_count_MEM2;

        curADC3Result = ADC14->MEM[2]; // store the analog voltage value
        ADC14->CTL0 |= ADC14_CTL0_SC; // start the next conversion in the sequence
        led_count_MEM2 = get_led_count(curADC3Result); // set the number of LEDs based on force
        if(led_count_MEM2 > 0) {
            start_led_effect = 1; // set the flag to trigger LED effect in timer ISR
            led_count = led_count_MEM2;
            GAME_PORT->OUT &= ~(GAME_OUT_3);
            countdown = 2;
        }
    }

    if (ADC14->IFGR0 & ADC14_IFGR0_IFG3) {

        uint8_t led_count_MEM3;

        curADC4Result = ADC14->MEM[3]; // store the analog voltage value
        led_count_MEM3 = get_led_count(curADC4Result); // set the number of LEDs based on force
        if(led_count_MEM3 > 0) {
            start_led_effect = 1; // set the flag to trigger LED effect in timer ISR
            led_count = led_count_MEM3;
            GAME_PORT->OUT &= ~(GAME_OUT_4);
            countdown = 2;
        }

        resultReady = true; // set to true once the sequence of conversions is completed
    }
}

void TA0_0_IRQHandler(void) {
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

    if(countdown > 0){
        if(countdown > 1) {
            countdown--;
        } else {
            turn_off_game_output();
            countdown = 0;
        }
    }

    if (start_led_effect || turning_off) {
            if (!turning_off) {  // Turning LEDs on phase
                if (current_led < led_count) {
                    LED_PORT->OUT |= LED_PINS[current_led];  // Turn on next LED
                    current_led++;
                } else {
                    turning_off = 1;  // Start turn-off sequence after holding
                    start_led_effect = 0; //
                }
            } else {  // Turning LEDs off phase
                if(start_led_effect){
                    LED_PORT->OUT = 0; // turn off all LEDs
                    turning_off = 0;
                    current_led = 0;
                    return;
                }
                if (hold_time > 0) {
                    hold_time--;  // Wait before turning LEDs off
                } else {
                    LED_PORT->OUT &= ~(LED_PINS[current_led]);  // Turn off LED
                    if (current_led == 0) {
//                        start_led_effect = 0;  // Reset after last LED turns off
                        turning_off = 0;
                        hold_time = HOLD_TIME;  // Reset hold time
                    } else {
                        current_led--;  // Move to the next LED to turn off
                    }
                }
            }
        }

}
