//####################################################
//#    KLOUBAK: Voltage_angle_measuring_module (Voltmetr_meterK21.1)
//#    Copyright (c) 2019 ROBOTIKA
//#    programmed by: Tomas Picha
//####################################################

// DESCRTIPTION: -  version for K2 robot 
//               -  20 Hz - sampling frequency
//               -  module measure voltage - 12, 36V and angle of rotation (analog value)
//               -  only refactoring of version - Voltmetr_meterK21.0
//               -  add CAN delays 20 micros (testing)
//               -  add average of angle values
//               -  add watchdog reset
// HW: Aruino Nano, DOPSAT NAZEV SENZORU PRO MERENI UHLU, MCP2515+TJA1050 CAN module

/*----------------------- DEPENDENCES ----------------------------------*/
#include <TimerOne.h>
#include <mcp_can.h>
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // wdt lib

/*----------------------- DEFINITION -----------------------------------*/
MCP_CAN CAN(10);
const int potenciometr_pin = A0;
const int voltege12_pin = A2;
const int voltege36_pin = A1;

float R1_12 = 470000.0;  //value of resistance of resistor in voltage divider
float R2_12 = 220000.0; //value of resistance of resistor in voltage divider
float R1_36 = 674000.0; //value of resistance of resistor in voltage divider  
float R2_36 = 67000.0; //value of resistance of resistor in voltage divider

int CANmessageID;
unsigned char len = 0;
byte buf_voltage12[4]={0,0,0,0};
byte buf_voltage36[4]={0,0,0,0};
byte buf_resistance[4] = {0,0,0,0};

int can_adress_resistance = 128; //80 hexa
int can_adress_voltage12 = 129;  //81 hexa
int can_adress_voltage36 = 130;  //82 hexa

unsigned int resist_potnciometr = 0;
float voltage12 = 0.0; 
float voltage36 = 0.0; 

bool timmer_flag = false;

/*----------------------- SETUP ----------------------------------------*/
void setup(){
  serial_initial();
  CAN_initial();
  timer1_init();
  wdt_enable(WDTO_120MS);  //wdt reset 60ms ; 120MS
  Serial.println("initialization successfully done!");

}

/*----------------------- LOOP ------------------------------------------*/
void loop(){
  if (timmer_flag == true) {
     timmer_flag = false;
     wdt_reset (); // Reset watchdog counter
     voltage_measurement();
     angle_measurement();
     CAN_messages_sending();
  }
}
