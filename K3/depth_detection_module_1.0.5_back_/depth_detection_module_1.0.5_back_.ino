//####################################################
//#    KLOUBAK: Depth-detection Module 1.0.5_back
//#    Copyright (c) 2019 ROBOTIKA
//#    programmed by: Tomas Picha, Jan Kaderabek
//####################################################

// timer1 registrs  
// DESCRTIPTION: - 30 Hz - frequency
//               - back senzors
//               - watchdog reset implementation
//               - change I2C frequency
//               - change -- setMeasurementTimingBudget
// HW: Aruino Nano, 2x VL53L1X, MCP2515+TJA1050 CAN module

/*----------------------- DEPENDENCES ----------------------------------*/
#include <VL53L1X.h>
#include <SPI.h> // knihovna pro SPI sbernici
#include <mcp_can.h>
#include <Wire.h>
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // wdt lib

/*----------------------- DEFINITION -----------------------------------*/
VL53L1X sensor_A;
VL53L1X sensor_B;
byte sensor_pin_A = 2;
byte sensor_pin_B = 3;
unsigned int distance_A = 0; // distance in [mm]
unsigned int distance_B = 0; // distance in [mm]

MCP_CAN CAN(10);
unsigned int CANmessageID;
unsigned char len = 0;
byte buf_distance[4]={0,0,0,0};
int can_adress_distance = 114; //72 hexa  

bool timmer_flag = false;
int preload_timer = 62411;// preload timer 65536-16MHz/256/5Hz --- 256 is prescaler; 65563 - 16 bit counter(Timer1); 20Hz - requiered frequency --- 62411 ; 10 hz = 59286; 5hz =53036 ; 2hz=34286

/*----------------------- SETUP ----------------------------------------*/
void setup(){
  serial_initial();
  i2c_initial();
  VL53L1X_initial();
  i2c_scanner();
  CAN_initial();
  timer1_init();  // setting of timer1
  wdt_enable(WDTO_120MS);  //wdt reset 60ms ; 120MS
  Serial.println(" - initialization successfully done!");
}

/*----------------------- LOOP ------------------------------------------*/
void loop(){
  VL53L1X_measuring();
  if (timmer_flag == true) {
    sensor_A.stopContinuous();
    sensor_B.stopContinuous();
    timmer_flag = false;
    wdt_reset (); // Reset watchdog counter
    //Serial.print("distance[mm] : ");
    //Serial.println(distance);
    CAN_message_sending_to_APU();
    //serial_data_print ();
    sensor_A.startContinuous(30);
    sensor_B.startContinuous(30);
  }
}
