//####################################################
//#    Battery_charger: Battery_charger1.4
//#    Copyright (c) 2019 ROBOTIKA
//#    programmed by: Tomas Picha, Jan Kaderabek
//####################################################


// DESCRTIPTION: -   max voltage limit set to 37.6 V
/*               -   voltage coraction table  0-25.0 [V] -- 1.0,
                                              25-28.0 [V]-- 0.98452012,
                                              28-38.0 [V]-- 0.98419244,
                                              38-43.0 [V] -- 0.98255515 */
//               -   add current cutoff limit to 1.0 A                                               

// HW: Aruino nano, current probe ACS712 20 A, relay

/*----------------------- DEPENDENCES ----------------------------------*/
#include <TimerOne.h>
#include <stdint.h> //due to wdt
#include <avr/wdt.h> // wdt lib
#include <TM74HC595Display.h> // 4bitov display

/*----------------------- DEFINITION -----------------------------------*/
byte voltage_pin = A1;
byte current_pin = A0;

byte relay_pin = 4; 
byte buzzer_pin = 5; 

byte SCLK = 7; //display
byte RCLK = 6;  //dispaly
byte DIO = 8; // display

TM74HC595Display disp(SCLK, RCLK, DIO); //display
unsigned char LED_0F[29]; //display

float r1 = 680300.0;  //value of resistance of resistor in voltage divider
float r2 = 68100.0; //value of resistance of resistor in voltage divider

float voltage = 0;
float voltage_max_limit = 37.6; //V
float histereze_voltage = 37.0; //V

float current = 0;
float current_cutoff_limit =1.0; //A

bool charged = false;

/*----------------------- SETUP ----------------------------------------*/
void setup() {
   pinouts_initial();
   serial_initial();
   characters_table_for_display();
   display_init();
   timer_init();
   charging();
   wdt_enable(WDTO_4S);  //wdt reset 60ms ; 120MS ; 250MS ; 500MS ; 1S ; 2S ; 4S ; 8s ;
   Serial.println("initialization successfully done!");
}

/*----------------------- LOOP ------------------------------------------*/
void loop() {
  display_voltage();
  display_current();
  write_to_serial();
}
