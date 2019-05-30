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
    for (int i=0;i<ReceiveArraySize;i++){
      if (CANmessageID == CANAdressReceiveTable[i][0]){
        CANAdressReceiveBuffer[i].resendAdress = CANAdressReceiveTable[i][1];
        for (int j=0;j<8;j++){
           CANAdressReceiveBuffer[i].resendBuf[j] = buf[j];
        } 
        CANAdressReceiveBuffer[i].CountMessge = CANAdressReceiveBuffer[i].CountMessge + 1;
      }
    }
  }else {
    if (STOpSTARt == false){
      for (int i=0;i<SendArraySize;i++){
        if (CANmessageID == CANAdressSendTable[i][0]){
          CANAdressSendBuffer[i].resendAdress = CANAdressSendTable[i][1];
          for (int j=0;j<8;j++){
           CANAdressSendBuffer[i].resendBuf[j] = buf[j];
          }
          CANAdressSendBuffer[i].CountMessge = CANAdressSendBuffer[i].CountMessge + 1;
        }
      }
    }
  }
  ClearBuffer();
}

/*--------------------------- Odesílání zpráv CAN----------------------------------------------------------------------------------*/

void CAN_MESSAGE_SenD(){
 
  for (int i=0;i<ReceiveArraySize;i++){
    if (CANAdressReceiveBuffer[i].CountMessge > 0){
      CAN.sendMsgBuf(0x0+CANAdressReceiveBuffer[i].resendAdress, 0, 8, CANAdressReceiveBuffer[i].resendBuf);
      //Serial.print(CANAdressReceiveBuffer[i].CountMessge);
    }
  }
  //Serial.println("---");
  if (STOpSTARt == false){
    for (int i=0;i<SendArraySize;i++){
      if (CANAdressSendBuffer[i]. CountMessge > 0){
        CAN.sendMsgBuf(0x00000+CANAdressSendBuffer[i].resendAdress, 1, 4, CANAdressSendBuffer[i].resendBuf);
        //Serial.print(CANAdressSendBuffer[i].CountMessge);
      }
    }
    //Serial.println("---");
  } else {
    for (int i=0;i<4;i++){
       CAN.sendMsgBuf(0x00000200+CANvescID[i], 1, 4, currentREVERS); // motorum se pošle brzdí proud
       CAN.sendMsgBuf(0x0020+CANvescID[i], 0, 4, currentREVERS);
     }
  }
  ClearMessageBuffer();
}

/*---------------------------- Clear Bridge Buffers -----------------------------------------------------------------------------------------*/

void ClearMessageBuffer(){
  for (int i=0;i<ReceiveArraySize;i++){
    CANAdressReceiveBuffer[i].resendAdress = 0;
    for (int j=0;j<8;j++){
      CANAdressReceiveBuffer[i].resendBuf[j] = 0;
    } 
    CANAdressReceiveBuffer[i].CountMessge = 0; 
  }
  for (int i=0;i<SendArraySize;i++){
      CANAdressSendBuffer[i].resendAdress = 0;
      for (int j=0;j<8;j++){
        CANAdressSendBuffer[i].resendBuf[j] = 0;
      }
      CANAdressSendBuffer[i].CountMessge = 0;
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

