

/************************************************************
 * Four Channel ADC reader for voltage dividers
 *
 * Description: Analog inputs are on A0, A1, A3, A4. Manually triggered
 * analog-to-digital conversions with interrupt on-completion.
 * 
 * Author:   Kallen Selby converted Colin Young's TI MSP432 to ESP32
 * Last-modified:   4/10/2025
 * 
 *                   ESP32
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


#include <Arduino.h>

/* Constants */
#define NUM_LEDS 5 // variable for the number of LEDs for force-feedback
#define UPDATE_DELAY 50000 // timer A ticks between LED updates
#define HOLD_TIME 8 // time the LEDs stay lit

// ARRAYS
const uint8_t LED_PINS[NUM_LEDS] = {13, 12, 14, 27, 26}; // 5 consecutive pins
const uint16_t LED_THRESHOLDS[NUM_LEDS] = {9000/4, 12000/4, 13500/4, 14250/4, 14750/4};
const uint8_t NUM_FSR_PINS = 4;
const uint8_t FSR_pins[NUM_FSR_PINS] = {36, 39, 34, 35};
const uint8_t GamePin = 17; //pin to send game commands on

/* Globals */
volatile uint8_t led_count = 0;  // Global variable to store how many LEDs light up
volatile uint8_t start_led_effect = 0; // Flag to trigger LED animation in TIMERA0 ISR
volatile uint8_t current_led = 0; // Track which LED is lighting up
volatile uint8_t turning_off = 0; // Flag to indicate turn-off sequence
volatile uint8_t hold_time = HOLD_TIME; // hold time before turning off LEDs
volatile uint8_t countdown = 0; // countdown for turning off the keycode
volatile uint8_t glove_punching = 0; // left glove is zero and right glove is one
volatile uint8_t current_FSR_pin = 0;

/* Statics */
static volatile uint16_t curADC0Result;
static volatile uint16_t curADC1Result;
static volatile uint16_t curADC3Result;
static volatile uint16_t curADC4Result;
static volatile bool resultReady = false;

struct ADCMessage {
  int channelIndex;
  int value;
};

QueueHandle_t adcQueue;
hw_timer_t* timer = nullptr;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// FUNCTION DECLARATIONS:
void configure_led_pins();
uint8_t get_led_count(uint16_t adc_val);
void configure_FSR_pins();
void gameInputHandler(void* param);
void IRAM_ATTR onTimer();

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  // PIN CONFIGURATION
  pinMode(GamePin, OUTPUT);
  digitalWrite(GamePin, LOW);
  configure_led_pins(); // Init LED pins
  configure_FSR_pins(); // Init FSR pins

  Serial.println("Finished with Configuration.");

  // Create queue and task on Core 1
  adcQueue = xQueueCreate(10, sizeof(ADCMessage));
  xTaskCreatePinnedToCore(gameInputHandler, "GameInputTask", 2048, nullptr, 1, nullptr, 1);

  Serial.println("Started Other Core");

  // Setup timer interrupt on Core 0
  timer = timerBegin(0, 80, true); // 1us ticks
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 2000, true); // 2ms = 500Hz scan rate
  timerAlarmEnable(timer);

  Serial.println("Timer Started");

}

void loop() {
  // put your main code here, to run repeatedly:
}

/* FUNCTION DECLARATOINS */

// Turns all LED pins to output mode
void configure_led_pins() {
  for(int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
}


uint8_t get_led_count(uint16_t adc_val) {
  uint8_t num_lit = 0;

  for(int i = 0; i < NUM_LEDS; i++){
      if(adc_val > LED_THRESHOLDS[i]){
          num_lit++;
      } else {
          break; // efficiency
      }
  }
  return num_lit;
}

void configure_FSR_pins() {  
  analogReadResolution(12);

  for(int i = 0; i < NUM_FSR_PINS; i++) {
    pinMode(FSR_pins[i], INPUT);
  }
}

// CORE 0 TIMER ISR
// Reads each ADC channel consecutively, then sends their values to the queue
//CANNOT HAVE SERIAL PRINTS IN HERE
void IRAM_ATTR onTimer() {
  int adc_val = analogRead(FSR_pins[current_FSR_pin]);

  ADCMessage msg = {current_FSR_pin, adc_val};
  
  // tells the other core that this is not a high priority task
  // if there is a high priority task switch to that
  BaseType_t xHPW = pdFALSE; 

  xQueueSendFromISR(adcQueue, &msg, &xHPW); // send the msg to the other core to deal with it

  current_FSR_pin = (current_FSR_pin + 1) % 4;

  if(xHPW) portYIELD_FROM_ISR(); // idk mane
}

// CORE 1 Game Logic
// Handles pin output
void gameInputHandler(void* param) {
  ADCMessage msg;

  while(true) {
    //Serial.println("Other Core here! Hi!");

    if(xQueueReceive(adcQueue, &msg, portMAX_DELAY)) {
      int channel = msg.channelIndex;
      int val = msg.value;

      

      //Serial.println("Value Read: " + val);

      //map the game logic here - use TTL
      if(val > LED_THRESHOLDS[1]) {
        Serial.print("Value Read in Core 1: ");
        Serial.println(val);

        digitalWrite(GamePin, HIGH);
        vTaskDelay(500);
        digitalWrite(GamePin, LOW);
      }


    }
    vTaskDelay(1);
  }
}