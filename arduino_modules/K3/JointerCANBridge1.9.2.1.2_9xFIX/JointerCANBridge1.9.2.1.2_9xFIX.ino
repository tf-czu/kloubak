//####################################################
//#    KLOUBAK: JointerCANBridge1.9.2.1.2_9xFIX
//#    Copyright (c) 2019 ROBOTIKA
//#    programmed by: Tomas Picha, Jan Kaderabek
//####################################################


// DESCRTIPTION: -  version for K2 and K3 robots 
//               -  difference between K2 and K3 is in two led pins - K2 : - stop_led = 10;  // RED
//                                                                         - ledPin = 11;    // GREEN
//                                                                  - K3 : - stop_led = 11;  // RED
//                                                                         - ledPin = 10;    // GREEN
//               -  20 Hz - sampling frequency 
//               -  version without incoming message buffer and translation realized by address subtraction
//               -  version with changed CountCycle()
//               -  change CAN masks and filters setting 
//               -  add watchdog
// HW: Aruino Due, MCP2515+TJA1050 CAN module

//########################################################################################################
//# Tables for CAN address conversion
//################################################
//################################################
//# Tabel-receiving data from vescs sending to apu
//################################################
//# received address_(from_vesc_adress)     converted address(to_apu)
//#     2305[dec]-0x901[hex]                   145[dec]-0x91[hex]
//#     2306[dec]-0x902[hex]                   146[dec]-0x92[hex]
//#     2307[dec]-0x903[hex]                   147[dec]-0x93[hex]
//#     2308[dec]-0x904[hex]                   148[dec]-0x94[hex]
//#     2309[dec]-0x905[hex]                   149[dec]-0x95[hex]
//#     2310[dec]-0x906[hex]                   150[dec]-0x96[hex]
//################################################
//################################################
//# Table-receiving data from apu sending to vescs
//################################################
//# received address_(from_apu)     converted address(to_vescs)
//#     17[dec]-0x11[hex]              257[dec]-0x101[hex]
//#     18[dec]-0x12[hex]              258[dec]-0x102[hex]
//#     19[dec]-0x13[hex]              259[dec]-0x103[hex]
//#     20[dec]-0x14[hex]              260[dec]-0x104[hex]
//#     21[dec]-0x15[hex]              261[dec]-0x105[hex]
//#     22[dec]-0x16[hex]              262[dec]-0x106[hex]
//#     33[dec]-0x21[hex]              513[dec]-0x201[hex]
//#     34[dec]-0x22[hex]              514[dec]-0x202[hex]
//#     35[dec]-0x23[hex]              515[dec]-0x203[hex]
//#     36[dec]-0x24[hex]              516[dec]-0x204[hex]
//#     37[dec]-0x25[hex]              517[dec]-0x205[hex]
//#     38[dec]-0x26[hex]              518[dec]-0x206[hex]
//#     49[dec]-0x31[hex]              769[dec]-0x301[hex]
//#     50[dec]-0x32[hex]              770[dec]-0x302[hex]
//#     51[dec]-0x33[hex]              771[dec]-0x303[hex]
//#     52[dec]-0x34[hex]              772[dec]-0x304[hex]
//#     53[dec]-0x35[hex]              773[dec]-0x305[hex]
//#     54[dec]-0x36[hex]              774[dec]-0x306[hex]
//########################################################################################################

/*----------------------- DEPENDENCES ----------------------------------*/
#include <mcp_can.h>
#include <DueTimer.h>

#define WDT_KEY (0xA5) /*KEY: Password -- Should be written at value 0xA5. 
                         Writing any other value in this field aborts the write operation */

void watchdogSetup(void) {/*** watchdogDisable (); ***/}

/*----------------------- DEFINITION -----------------------------------*/
const int stop_led = 11;  // RED
const int break_led = 12; // BLUE
const int ledPin = 10;    // GREEN
const int Syrena = 8;

//byte stop_xbee = 22;
byte StopButton = 24;
byte control_break = 26;

MCP_CAN CAN(2);

int CANvescID[4] = {1,2,3,4};  // ID jednotlivých motorů
byte currentREVERS[4]={0,0,250,0}; // brzdící proud  
byte current_no_brake[4]={0,0,0,0}; // no brake current  

bool STOpSTARt = true; //pokud je nastaven na true je stop
bool digitRead = false;
bool brake_control = true; // pokud je nastaven na true tak brzdí
bool digit_read_brake = false;
bool Timmer4End = false;
bool Timer3Over = false; //indikace přetečení timeru3

byte CycleCount = 0; 
int  CANmessageID;
unsigned char len = 0;
byte buf[8];

/*----------------------- SETUP ----------------------------------------*/
void setup() {
 
  serial_initial();
  
  pin_mode_initial();
  pin_out_setting();
  
  ButtonRead();
  brake_button_read();
  timer3_4_initial();
  CAN_initial();
  LEDIndicator ();
  BeepUP();
  waiting_for_timer4_overflow();
  wdt_initial();
  Timer3.start(50000); // Calls every 50 mikros (20 Hz)
}

/*----------------------- LOOP ------------------------------------------*/
void loop() {
  ButtonRead();
  brake_button_read();
  if (Timer3Over == true){
    //Serial.println(STOpSTARt);
    CountCycle();
    SendButtonStatusAPU();
    if (STOpSTARt == true) {
      STOP();
    }
    Timer3Over = false;
    wdt_reset();
  }
  
  if(CAN_MSGAVAIL == CAN.checkReceive()) {  
    CAN.readMsgBuf(&len, buf);
    CANmessageID = CAN.getCanId();
    //Serial.println("in loop");
    //Serial.println(CANmessageID);
    //CANmsgToSerial();
    CAN_MESSAGE_ReceiveD();
  }
}
