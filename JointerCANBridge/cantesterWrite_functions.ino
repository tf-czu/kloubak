void Motor(){
  if (STOpSTARt == true) { // pokud je STOpSTAR == true takje stisklé Stop Button
     for (int i=0;i<4;i++){
        CAN.sendMsgBuf(0x00000200+CANvescID[i], 1, 4, currentREVERS); // motorum se pošle brzdí proud
        CAN.sendMsgBuf(0x0020+CANvescID[i], 0, 4, currentREVERS);
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

/*---------------------------StopButton--------------------------------------------------------------------------------------*/

void ButtonRead(){
  digitRead = digitalRead(StopButton);
  if (digitRead==true){
    STOpSTARt = true;
  } else {
    STOpSTARt = false;
  }
}

/*---------------------------- Zašle stav tlačítek apu--------------------------------------------------------------------------*/

void SendButtonStatusAPU() {
  byte messageStopButt[1];
   if (STOpSTARt == false){
      messageStopButt[0]=0;
      CAN.sendMsgBuf(0x1, 0, 1, messageStopButt);  // pokud je messageStopButt=0 Stop tlačítko není zmáčknuté
    } else {
        messageStopButt[0]=1;
        CAN.sendMsgBuf(0x1, 0, 1, messageStopButt);  // pokud je messageStopButt=1 Stop tlačítko není zmáčknuté
      }
}

/*--------------------------- Přijatá zpráva CAN----------------------------------------------------------------------------------*/

void CAN_MESSAGE_ReceiveD(){
  if (CANmessageID > 2304){
    for (int i=0;i<4;i++){
      if (CANmessageID == CANAdressReceiveTable[i][0]){
        CANAdressReceiveBuffer[i][0]= CANAdressReceiveTable[i][1];
        CANAdressReceiveBuffer[i][1]=int(buf);
        CANAdressReceiveBuffer[i][2]=1;
      }
    }
  }else {
    if (STOpSTARt == false){
      for (int i=0;i<8;i++){
        if (CANmessageID == CANAdressSendTable[i][0]){
          CANAdressSendBuffer[i][0]= CANAdressSendTable[i][1];
          CANAdressSendBuffer[i][1]=int(buf);
          CANAdressSendBuffer[i][2]=1;
        }
      }
    }
  }
  ClearBuffer();
}

/*--------------------------- Odesílání zpráv CAN----------------------------------------------------------------------------------*/

void CAN_MESSAGE_SenD(){
  byte buffPom[1];
  
  for (int i=0;i<4;i++){
    if (CANAdressReceiveBuffer[i][2] == 1){
      buffPom[0]= byte(CANAdressReceiveBuffer[i][1]);
      CAN.sendMsgBuf(0x0+CANAdressReceiveBuffer[i][0], 0, 8, buffPom);
    }
  }
  if (STOpSTARt == false){
    for (int i=0;i<8;i++){
      if (CANAdressSendBuffer[i][2] == 1){
        buffPom[0]= byte(CANAdressSendBuffer[i][1]);
        CAN.sendMsgBuf(0x00000+CANAdressSendBuffer[i][0], 1, 4, buffPom );
      }
    }
  }
  ClearCANBuffers();
}

/*---------------------------- Clear Bridge Buffers -----------------------------------------------------------------------------------------*/

void ClearCANBuffers(){
  for (int i=0;i<ReceiveArraySize[0][0];i++){
    for (int j=0;j<ReceiveArraySize[0][1];j++){
      CANAdressReceiveBuffer[i][j] = 0; 
    }
  }
  for (int i=0;i<SendArraySize[0][0];i++){
    for (int j=0;j<SendArraySize[0][1];j++){
      CANAdressSendBuffer[i][j] = 0; 
    }
  }
}

/*---------------------------- Cycle count --------------------------------------------------------------------------------------------------*/

void CountCycle(){
  byte bufCount[1];

  bufCount[0]= CycleCount;
  CAN.sendMsgBuf(0x02, 0, 1, bufCount);
  if (CycleCount==255){
    CycleCount = 0;
  } else {
     CycleCount = CycleCount+1;
    }
  
}

