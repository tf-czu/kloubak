//####################################################
//#    KLOUBAK: Voltage_angle_measuring_module (Voltmetr_meterK31.1)
//#    Copyright (c) 2019 ROBOTIKA
//#    programmed by: Tomas Picha, Jan Kaderabek
//####################################################

// DESCRTIPTION: -  version for K3 robot 
//               -  20 Hz - sampling frequency
//               -  module measure voltage - 12, 36V and angle of rotation (analog value)
//               -  refactoring of version - Voltmetr_meterK21.0 + new update for K3
//               -  angle sensors (I2C) - update
//               - change I2C frequency to 100 kHz
//               - corected Voltage meassurment
//               - add testing CAN delays (20 micros)
//               - add watchdog reset
// HW: Aruino Nano, 2 x AS5048A(angle sensors), MCP2515+TJA1050 CAN module

/*----------------------- DEPENDENCES ----------------------------------*/
#include <mcp_can.h>
#include <Wire.h>
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // wdt lib

/*----------------------- DEFINITION -----------------------------------*/
MCP_CAN CAN(10);
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
byte buf_angles[4] = {0,0,0,0};

int can_adress_angles = 128; //80 hexa
int can_adress_voltage12 = 129;  //81 hexa
int can_adress_voltage36 = 130;  //82 hexa

byte sensor1_i2c_adrress = 64; // 0x40
byte sensor2_i2c_adrress = 65; // 0x41

byte angle_254 = 0;
byte angle_255 = 0;

float voltage12 = 0.0; 
float voltage36 = 0.0; 

bool timmer_flag = false;
int preload_timer = 62411;// preload timer 65536-16MHz/256/5Hz --- 256 is prescaler; 65563 - 16 bit counter(Timer1); 20Hz - requiered frequency --- 62411 ; 10 hz = 59286; 5hz =53036 ; 2hz=34286

/*----------------------- SETUP ----------------------------------------*/
void setup(){
  serial_initial();
  CAN_initial();
  I2C_initial();
  I2C_scanner();
  timer1_init();  // setting of timer1
  wdt_enable(WDTO_120MS);  //wdt reset 60ms ; 120MS
  Serial.println("initialization successfully done!");

}

/*----------------------- LOOP ------------------------------------------*/
void loop(){
  if (timmer_flag == true) {
     timmer_flag = false;
     wdt_reset (); // Reset watchdog counter
     voltage_measurement();
     angles_measurement();
     CAN_messages_sending();
  }
}
