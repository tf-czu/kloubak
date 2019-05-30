void Motor(){
  /*Serial.print(RPM[0]);
  Serial.print(";");
  Serial.println(RPM[1]);*/
  ReceveCANMSG();
  if (Motorflag == 1) {
    if (RPM[CANvescID[0]]< RPMStart) {
       CAN.sendMsgBuf(0x00000100+CANvescID[0], 1, 4, currentSTART);
    } else if (RPM[CANvescID[0]]<RPMMIN1){
        CAN.sendMsgBuf(0x00000100+CANvescID[0], 1, 4, currentONM1);
        RPMMAXFlag1 = false;
        RPMMINFlag1 = true;
      } else if (RPM[CANvescID[0]]>RPMMAX1) {
          CAN.sendMsgBuf(0x00000100+CANvescID[0], 1, 4, currentOFF);
          RPMMAXFlag1 = true;
          RPMMINFlag1 = false;
        } else if (RPM[CANvescID[0]]<=RPMMAX1 && RPM[CANvescID[0]]>=RPMMIN1) {
            if (RPMMAXFlag1 == true){
               CAN.sendMsgBuf(0x00000100+CANvescID[0], 1, 4, currentOFF); 
             } else if (RPMMINFlag1 == true){
                  CAN.sendMsgBuf(0x00000100+CANvescID[0], 1, 4, currentONM1);
                }  
             }
    if (RPM[CANvescID[1]]< RPMStart) {
       CAN.sendMsgBuf(0x00000100+CANvescID[1], 1, 4, currentSTART);
    } else if (RPM[CANvescID[1]]<RPMMIN2){
        CAN.sendMsgBuf(0x00000100+CANvescID[1], 1, 4, currentONM2);
        RPMMAXFlag2 = false;
        RPMMINFlag2 = true;
      } else if (RPM[CANvescID[1]]>RPMMAX2) {
          CAN.sendMsgBuf(0x00000100+CANvescID[1], 1, 4, currentOFF);
          RPMMAXFlag2 = true;
          RPMMINFlag2 = false;
        } else if (RPM[CANvescID[1]]<=RPMMAX2 && RPM[CANvescID[1]]>=RPMMIN2) {
            if (RPMMAXFlag2 == true){
               CAN.sendMsgBuf(0x00000100+CANvescID[1], 1, 4, currentOFF); 
             } else if (RPMMINFlag2 == true){
                  CAN.sendMsgBuf(0x00000100+CANvescID[1], 1, 4, currentONM2);
                }  
             }
    
    }else{
      for (int i=0;i<2;i++){
        if (RPM[CANvescID[i]]>0) {
           CAN.sendMsgBuf(0x00000200+CANvescID[i], 1, 4, currentREVERS);
        }else {
           CAN.sendMsgBuf(0x00000100+CANvescID[i], 1, 4, currentOFF);
        }
     }
   }
}

void CANmsgToSerial(){
      Serial.println("-----------------------------------------");
      Serial.print("can address: ");
      Serial.print(CANmessageID);
      Serial.print("; lenght: ");
      Serial.println(len);
      for (int i=0;i<len;i++){
        Serial.print(buf[i]);
        Serial.print("\t");
      }
      Serial.println("");
      Serial.println("-----------------------------------------");
}

/*--------------------------CAN-MSG-Recev---------------------------------------------*/

void CAN_MSG_Recev() {
  int VescID = -1;
  
  if (CANmessageID == 2305){ // zkontrolovat správnost adresy
    VescID = 1;
  }
  if (CANmessageID == 2306){ // zkontrolovat správnost adresy
    VescID = 2;
  }
  /*if (CANmessageID == 2307){ // zkontrolovat správnost adresy
    VescID = 3;
  }*/
  CAN_MSG_RPM(VescID);
}

/*-------------------------- CAN-MSG-RPM----------------------------------------------*/

void CAN_MSG_RPM(int VescID) {
  int RPMpom[4]={0,0,0,0};
  
  RPM[VescID] = 0;
  for (int i=2;i<4;i++) {
    RPMpom[i] = buf[i] * pow(256,(3-i)) ;
    //Serial.println(RPMpom[i]);
  }
  for (int i=2;i<4;i++) {
    RPM[VescID] = RPM[VescID] + RPMpom[i];
  }
  RPM[VescID]= RPM[VescID]/3;
}

/*-------------------------- Probuzeni arduina od interniho preruseni ----------------*/

void TimerInterrupt () {
  Timer3Over = true;
}

/*---------------------------Led indikace ----------------------------------------------------------------*/
void LEDIndicator () {
  for (int i=0; i <= 3; i++) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
  }
}

/*------------------------- Clear Buffer ------------------------------------------------------------------------*/

void ClearBuffer ()
{
  for (int i=0; i <= 7; i++) {
      buf[i] = 0;
  }
}

/*-------------------------- SyrenaDown ------------------------------------------------------------------------------*/

void BeepDown () {
  
  Timer4.stop();
  digitalWrite(Syrena, LOW);
  digitalWrite(ledPin, LOW);
  Timmer4End = true;
}

/*-------------------------- SyrenaUP ------------------------------------------------------------------------------*/

void BeepUP () {
  
  digitalWrite(Syrena, HIGH);
  digitalWrite(ledPin, HIGH);
  Timer4.start(300000);
}

/*----------------------------LOG------------------------------------------------------------------------------------------*/

void LOG() {
  Serial.print ("Motorflag: ");
  Serial.println(Motorflag);
  Serial.print("STOP Button: ");
  Serial.println(STOpSTARt);
  Serial.print("RPM[1]: ");
  Serial.println(RPM[1]);
  Serial.print("RPM[2]: ");
  Serial.println(RPM[2]);
  Serial.print("RPM[3]: ");
  Serial.println(RPM[3]);
  Serial.println("-----------------------------------------");
}

/*---------------------------StopButton--------------------------------------------------------------------------------------*/

void ButtonRead(){
  digitRead = digitalRead(StopButton);
  if (digitRead==true){
    STOpSTARt = -1;
  } else {
    STOpSTARt = 1;
  }
}

/*-----------------------receve CanMessage-------------------------------------------------------------------------------------*/

void ReceveCANMSG() {
  bool CanReceveMSGPom = false;
  while(CAN_MSGAVAIL != CAN.checkReceive()||CanReceveMSGPom==false){
     CAN.readMsgBuf(&len, buf);
     CANmessageID = CAN.getCanId();
     if (CANmessageID == 2305){
        CAN_MSG_Recev();
        CanReceveMSGPom = true;
     }
  }
  while(CAN_MSGAVAIL == CAN.checkReceive()||CanReceveMSGPom==false){
     CAN.readMsgBuf(&len, buf);
     CANmessageID = CAN.getCanId();
     if (CANmessageID == 2306){
        CAN_MSG_Recev();
        CanReceveMSGPom = true;
     }
  }
 /* while(CAN_MSGAVAIL == CAN.checkReceive()||CanReceveMSGPom==false){
     CAN.readMsgBuf(&len, buf);
     CANmessageID = CAN.getCanId();
     if (CANmessageID == 2307){
        CAN_MSG_Recev();
        CanReceveMSGPom = true;
     }
  } */
}

