//####################################################
//#    KLOUBAK: Sirene Module K2 1.0
//#    Copyright (c) 2019 ROBOTIKA
//#    programmed by: Tomas Picha
//####################################################

// DESCRTIPTION: -  version for K2 robot 
//               -  no tested
//               -  20 Hz - sampling frequency
//               -  timer1 registrs 
//               -  CAN_mask and filter settings 
//               -  add watchdog reset
// HW: Aruino Due, MCP2515+TJA1050 CAN module, sirene

// version

/*----------------------- DEPENDENCES ----------------------------------*/
#include <mcp_can.h>
#include <SPI.h> // lib for SPI bus
#include <DueTimer.h>

#define WDT_KEY (0xA5) /*KEY: Password -- Should be written at value 0xA5. 
                         Writing any other value in this field aborts the write operation */

void watchdogSetup(void) {/*** watchdogDisable (); ***/}

/*----------------------- DEFINITION -----------------------------------*/
const int led_pin =  A0;
const int buzzer_pin = 8;
const int load_pin_1=3;
const int load_pin_2=4;
const int load_pin_3=5;
const int cs_pin = 2;

MCP_CAN CAN(cs_pin);

const int can_adress_receive = 96;  // receiveing can adress 0X60
const int can_adress_transmit = 97; // transmiting can adress  0X61
int  CANmessageID; // can adress of incoming message
byte buf[8];
byte transmit_buf[1];
unsigned char len = 0;
unsigned char len_incaming_message = 1;
const int receive_message_lenth = 1;

//const int syrene_time_on = 300000;
bool sirene_reguest_on = false;
bool sirene_reguest_off = false;

byte cycle_count = 0;

bool timer3_over = false; //indication of timer overflow
bool timmer4_end = false;

/*----------------------- SETUP ----------------------------------------*/
void setup() {
  serial_initial();
  CAN_initial();
  pin_mode_initial();
  pin_out_setting();
  timer3_4_initial();
  led_indicator ();
  beep_up ();
  waiting_for_timer4_overflow();
  wdt_initial();
  Timer3.start(50000); // Calls every 50 mikros (20 Hz)
}

/*----------------------- LOOP ------------------------------------------*/
void loop() {
  if (timer3_over == true){
    timer3_over = false;
    wdt_reset(); // Reset watchdog counter
    sending_hb_apu();
    cycle_count_service();    
  }

  checkout_can_incoming_message();
  if (sirene_reguest_on == true){
    sirene_reguest_on = false;
    sirene_on();
  }
  if (sirene_reguest_off == true){
    sirene_reguest_off = false;
    sirene_off();
  }
}
