//####################################################
//#    KLOUBAK: Sirene Module K3 1.0
//#    Copyright (c) 2019 ROBOTIKA
//#    programmed by: Tomas Picha
//####################################################

// DESCRTIPTION: -  version for K3 robot 
//               -  no tested
//               -  20 Hz - sampling frequency
//               -  timer1 registrs 
//               -  CAN_mask and filter settings 
//               -  add watchdog reset
// HW: Aruino nano, buzzer, 8xled indication, MCP2515+TJA1050 CAN module

// version

/*----------------------- DEPENDENCES ----------------------------------*/
#include <mcp_can.h>
#include <SPI.h> // lib for SPI bus
#include <TimerOne.h>
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // wdt lib

/*----------------------- DEFINITION -----------------------------------*/
const byte led_pinout_count = 8;  // count of led pins 
const int buzzer_pin = 17;
const byte cs_pin = 10;
byte led_pinout[led_pinout_count]= {7,6,4,3,5,9,2,8};

MCP_CAN CAN(cs_pin);

const int can_adress_receive = 96;  // receiveing can adress 0X60
const int can_adress_transmit = 97; // transmiting can adress  0X61
int  CANmessageID; // can adress of incoming message
byte buf[8];
byte transmit_buf[1];
unsigned char len = 0;
unsigned char len_incaming_message = 1;
const int receive_message_lenth = 1;

const int leds_time_on = 60;// count of timer1 interupts if one interrupt is set to 50 mikros (20 Hz)
bool leds_reguest_on = false;
bool leds_reguest_off = false;

byte cycle_count = 0;

bool led_timer_start = false;
int led_timer_count =0; //count of 
bool timer1_over = false; //indication of timer overflow
bool timmer3_end = false;

/*----------------------- SETUP ----------------------------------------*/
void setup() {
  serial_initial();
  CAN_initial();
  pinout_initial();
  beep();
  knight_rider_led_test ();
  wdt_enable(WDTO_120MS);  //wdt reset 60ms ; 120MS
  timer1_init();  // setting of timer1
}

/*----------------------- LOOP ------------------------------------------*/
void loop() {
  if (timer1_over == true){
    timer1_over = false;
    wdt_reset (); // Reset watchdog counter
    sending_hb_apu();
    if (led_timer_start == true) {
      led_timer_check_status();
    }
    cycle_count_service();    
  }

  checkout_can_incoming_message();
  if (leds_reguest_on == true){
    leds_reguest_on = false;
    leds_on();
  }
  if (leds_reguest_off == true){
    leds_reguest_off = false;
    leds_off();
  }
}
