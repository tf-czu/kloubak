/* modul - měří naoětí 12V, 36V, odpor na potaku - K2*/

#include <TimerOne.h>

float R1_12 = 470000.0; //  
float R2_12 = 220000.0; //
float R1_36 = 674000.0; //  
float R2_36 = 67000.0; //

#include <mcp_can.h>

MCP_CAN CAN(10);
int           CANmessageID;
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

const int potenciometr_pin = A0;
const int voltege12_pin = A2;
const int voltege36_pin = A1;

bool timmer_flag = false;


void setup(){
  Serial.begin(250000); 
  while (!Serial) {
    ; // ceka dokud neni pripojena seriova linka
    }

  Serial.println("Setup can..");
  while(CAN.begin(CAN_500KBPS, MCP_8MHz) != CAN_OK){
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nCAN init ok!!\r\n");

  Timer1.initialize(50000);
  Timer1.attachInterrupt(timer_overflow);

  Serial.println("Inicializace -- OK");

}
void loop(){
  if (timmer_flag == true) {
   voltage12 =  Voltage (voltege12_pin,R1_12, R2_12);
   voltage36 =  Voltage (voltege36_pin,R1_36, R2_36);
   timmer_flag = false;
   voltage_message12();
   CAN.sendMsgBuf(0x00 + can_adress_voltage12, 0, 4, buf_voltage12);
   voltage_message36();
   CAN.sendMsgBuf(0x00 + can_adress_voltage36, 0, 4, buf_voltage36);
   resist_potnciometr = potenciomter(potenciometr_pin);
   resistance_message();
   CAN.sendMsgBuf(0x00 + can_adress_resistance, 0, 4, buf_resistance);
   //Serial.print("Current = ");
   //Serial.println(celk_proud,3);
  }
}

