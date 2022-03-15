// version -- if statement for vesc canID and new tacho_buf indexing and timer change to 20 Hz
// version -- for arduino nano

#include <mcp_can.h>
#include <SPI.h> // knihovna pro SPI sbernici


const int can_vesc_count = 4; // count of vescs on the robot
int CANvescID[can_vesc_count] = {1,2,3,4};  // ID jednotlivých motorů

MCP_CAN CAN(10);

int can_adress_transmit = 131; // can id odchozí zprávy 0X83
int  CANmessageID;
byte buf[8];
unsigned char len = 0;
byte tacho_buf[8] = {0,0,0,0,0,0,0,0};
byte request_buf[3] = {0,0,4}; 

int tacho_bufer_index_tabel[4]={1,3,5,7};

bool Timer3Over = false; //indikace přetečení timeru3
int preload_timer = 62411;// preload timer 65536-16MHz/256/5Hz --- 256 is prescaler; 65563 - 16 bit counter(Timer1); 20Hz - requiered frequency --- 62411 ; 10 hz = 59286; 5hz =53036 ; 2hz=34286

void setup() {
 
  Serial.begin(250000);

  Serial.println("Setup can..");
  while(CAN.begin(CAN_500KBPS, MCP_8MHz) != CAN_OK){
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nCAN init ok!!\r\n");
   
   // pro jine verye aruduino }jinak nepreklada]  
  CAN.init_Mask(0, 1, 0x00000600); //CAN.init_Mask(0, 1, 0x06FF0000);
  CAN.init_Mask(1, 1, 0x00000600);
  CAN.init_Filt(0, 1, 0x00000501);
  CAN.init_Filt(1, 1, 0x00000502);
  CAN.init_Filt(2, 1, 0x00000503);
  CAN.init_Filt(3, 1, 0x00000504);

  timer1_init();  // setting of timer1

  Serial.println("Inicializace OK");
}

void loop() {
  if (Timer3Over == true){
    Timer3Over = false;
    for (int i=0;i<can_vesc_count;i++){
      CAN_MESSAGE_ReceiveD(i);
    }
    CAN.sendMsgBuf(0x0+can_adress_transmit, 0, 8, tacho_buf); // sending tacho infos to APU    
  }
}
