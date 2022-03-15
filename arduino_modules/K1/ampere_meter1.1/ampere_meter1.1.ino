/*modul - měří proud, odpor na potaku na K1*/

#include <TimerOne.h>
#include <mcp_can.h>

float celk_proud; // proud v obvodu se zatezi

const int led = 8;

MCP_CAN CAN(10);
int           CANmessageID;
unsigned char len = 0;
byte buf_current[3]={0,0,0};
byte buf_resistance[4] = {0,0,0,0};

int can_adress_current = 112;  //70 hexa
int can_adress_resistance = 128; //80 hexa

unsigned int resist_potnciometr = 0; 

const int potenciometr_pin = A1;
const int current_pin = A0;

bool Timmer1End = false;

bool timmer_flag = false;


void setup(){
  Serial.begin(250000); 
  while (!Serial) {
    ; // ceka dokud neni pripojena seriova linka
    }

  pinMode(led, OUTPUT);

  Serial.println("Setup can..");
  while(CAN.begin(CAN_500KBPS, MCP_8MHz) != CAN_OK){
    digitalWrite(led,HIGH);
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nCAN init ok!!\r\n");

  Timer1.attachInterrupt(BeepDown);
  
  BeepUP ();
  
  while(Timmer1End != true){
    delay(100);
  }
  digitalWrite(led, LOW);
  
  Timer1.initialize(50000);
  Timer1.attachInterrupt(timer_overflow);

  Serial.println("Inicializace -- OK");

}
void loop(){
  if (timmer_flag == true) {
   celk_proud = proud20A (current_pin);
   timmer_flag = false;
   current_message();
   CAN.sendMsgBuf(0x00 + can_adress_current, 0, 3, buf_current);
   resist_potnciometr = potenciomter(potenciometr_pin);
   resistance_message();
   CAN.sendMsgBuf(0x00 + can_adress_resistance, 0, 4, buf_resistance);
   //Serial.print("Current = ");
   //Serial.println(celk_proud,3);
  }
}

