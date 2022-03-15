
/*verze bez bufru příchozích zpráv a překlad realizován odečítáním adres
 verze pro K1*/

#include <mcp_can.h>
#include <DueTimer.h>

const int ledPin =  A0;
const int Syrena = 8;


MCP_CAN CAN(2);

byte CycleCount = 0; 

int           CANmessageID;
unsigned char len = 0;
byte buf[8];

int CANvescID[4] = {1,2,3,4};  // ID jednotlivých motorů

/*int CANAdressReceiveTable[4][2]={{2305,145}, //{přijatá hodntoa,přeposílaná hodntota} -- to co se má jen přeposílat APU
                                 {2306,146},
                                 {2307,147},
                                 {2308,148}}; 
int CANAdressSendTable[12][2]={{17,257},//{přijatá hodntoa,přeposílaná hodntota} -- to co se má posílat vESCum
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

byte currentOFF[4]={0,0,0,0}; // tohle chce reverzovat, ma to znamenat 0mA
byte currentREVERS[4]={0,0,250,0}; // brzdící proud  

bool STOpSTARt = true; //pokud je nastaven na true je stop
byte StopButton = 24;
bool digitRead = false;

bool Timmer4End = false;

bool Timer3Over = false; //indikace přetečení timeru3

void setup() {
 
  Serial.begin(250000);

  pinMode(ledPin, OUTPUT);
  pinMode(Syrena,OUTPUT); //syrena
  
  digitalWrite(ledPin, LOW);
  digitalWrite(Syrena, LOW);

  pinMode(StopButton,INPUT_PULLUP);
   
  Serial.println("Setup can..");
  while(CAN.begin(CAN_500KBPS, MCP_8MHz) != CAN_OK){
    digitalWrite(ledPin, HIGH);
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nCAN init ok!!\r\n");

  LEDIndicator ();

  Timer3.attachInterrupt(TimerInterrupt);

  Timer4.attachInterrupt(BeepDown);

  BeepUP ();
  
  while(Timmer4End != true){
    delay(100);
  }

  Timer3.start(50000); // Calls every 50 mikros (20 Hz)
}


void loop() {
  ButtonRead();
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
    //CANmsgToSerial();
    CAN_MESSAGE_ReceiveD();
  }
}

