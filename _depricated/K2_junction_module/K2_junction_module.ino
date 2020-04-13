/////////////////////////////////////////////////////////////////////
/*    - K2 -     */
/*verze bez bufru příchozích zpráv a překlad realizován odečítáním adres*/
/* verze pro K2*/

/////////////////////////////////////////////////////////////////////
/*{přijatá hodntoa,přeposílaná hodntota} -- to co se má jen přeposílat APU*/
/*int CANAdressReceiveTable[4][2]={{2305,145},
                                 {2306,146},
                                 {2307,147},
                                 {2308,148}}; 
/*{přijatá hodntoa,přeposílaná hodntota} -- to co se má posílat vESCum*/
/*int CANAdressSendTable[12][2]={{17,257},
                              {18,258},
                              {19,259},
                              {20,260},
                              {33,513}, //neni pozmeneno !!! 
                              {34,514}, //neni pozmeneno !!!
                              {35,515},
                              {36,516},
                              {49,769},
                              {50,770},
                              {51,771},
                              {52,772}};*/
/////////////////////////////////////////////////////////////////////

#include <mcp_can.h>
#include <DueTimer.h>

const int stop_led = 10;  // RED
const int break_led = 12; // BLUE
const int ledPin = 11;    // GREEN
const int Syrena = 8;

byte stop_xbee = 22; // for K2
byte StopButton = 24;
byte control_break = 26; // for K2

int CANvescID[4] = {1,2,3,4};  // ID jednotlivých motorů
byte currentREVERS[4]={0,0,250,0}; // brzdící proud  
byte current_no_brake[4]={0,0,0,0}; // no brake current  

MCP_CAN CAN(2);

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

void setup() {
 
  Serial.begin(250000);

  pinMode(ledPin, OUTPUT);
  pinMode(Syrena,OUTPUT); //syrena
  pinMode(stop_led, OUTPUT);
  pinMode(break_led, OUTPUT);

  
  digitalWrite(ledPin, LOW);
  digitalWrite(Syrena, LOW);
  ButtonRead();
  brake_button_read();

  pinMode(StopButton,INPUT_PULLUP);
  pinMode(control_break,INPUT_PULLUP);

  Timer3.attachInterrupt(TimerInterrupt);
  Timer4.attachInterrupt(BeepDown);
   
  Serial.println("Setup can..");
  while(CAN.begin(CAN_500KBPS, MCP_8MHz) != CAN_OK){
    digitalWrite(stop_led, HIGH);
    Serial.print(".");
    BeepUP();
    delay(1000);
  }
  Serial.print("\nCAN init ok!!\r\n");
  LEDIndicator ();
  BeepUP();
   
  while(Timmer4End != true){
    delay(100);
   // pro jine verye aruduino }jinak nepreklada]  
  CAN.init_Mask(0, 1, 0x06FF0000);
  CAN.init_Filt(0, 1, 0x00000901);
  CAN.init_Filt(1, 1, 0x00000902);
  CAN.init_Filt(2, 1, 0x00000903);
  CAN.init_Filt(3, 1, 0x00000904);
  }

  Timer3.start(50000); // Calls every 50 mikros (20 Hz)
}

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

