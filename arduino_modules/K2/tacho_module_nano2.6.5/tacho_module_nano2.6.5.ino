//####################################################
//#    KLOUBAK: Tacho Module 2.6.5
//#    Copyright (c) 2019 ROBOTIKA
//#    programmed by: Tomas Picha, Martin Dlouhy
//####################################################


// DESCRTIPTION: -  version for and K2 robot 
//               -  version for VESC firmware 5.2
//               -  20 Hz - sampling frequency
//               -  timer1 registrs
//               -  only refactoring of version - Tacho Module 2.5.5
//               -  new can Masks and filtres settings
//               -  add watchdog reset
// HW: Aruino Nano, MCP2515+TJA1050 CAN module

// version -- if statement for vesc canID and new tacho_buf indexing and timer change to 20 Hz

/*----------------------- DEPENDENCES ----------------------------------*/
#include <mcp_can.h>
#include <SPI.h> // lib for SPI bus
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // wdt lib

/*----------------------- DEFINITION -----------------------------------*/
const int can_vesc_count = 4; // count of vescs on the robot
int CANvescID[can_vesc_count] = {1,2,3,4};  // ID jednotlivých motorů

MCP_CAN CAN(10);

int can_adress_transmit = 131; // can id odchozí zprávy 0X83
int can_adress_transmit_error = 132; // can id odchozí zprávy 0X84
int  CANmessageID;
byte buf[8];
unsigned char len = 0;
byte CAN_error = 0; //checkong CAN error if 0 --> no error on CAN BUS; if 1 ---> error on CAN BUS is indicated
byte tacho_buf[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
byte request_buf[7] = {0,0,50,0,0,32,1}; 
byte error_buf[2] = {0,0}; 

int tacho_bufer_index_tabel[4]={1,3,5,7};

bool Timer3Over = false; //indication of timer overflow
int preload_timer = 62411;// preload timer 65536-16MHz/256/5Hz --- 256 is prescaler; 65563 - 16 bit counter(Timer1); 20Hz - requiered frequency --- 62411 ; 10 hz = 59286; 5hz =53036 ; 2hz=34286

byte counter = 0; //cycle counter

/*----------------------- SETUP ----------------------------------------*/
void setup() {
 
  serial_initial();
  CAN_initial();
  timer1_init();  // setting of timer1
  wdt_enable(WDTO_120MS);  //wdt reset 60ms ; 120MS
  Serial.println("initialization done!");
}

/*----------------------- LOOP ------------------------------------------*/
void loop() {
  if (Timer3Over == true){
    Timer3Over = false;
    wdt_reset (); // Reset watchdog counter
    //counter_cycle();
    tacho_buffer_clear();
    for (int i=0;i<can_vesc_count;i++){
      CAN_MESSAGE_ReceiveD(i);
    }
    CAN.sendMsgBuf(0x0+can_adress_transmit, 0, 8, tacho_buf); // sending tacho infos to APU
    //CAN.sendMsgBuf(0x0+can_adress_transmit_error, 0, 2, error_buf); // sending error on CAN infos to APU     
  }
}
