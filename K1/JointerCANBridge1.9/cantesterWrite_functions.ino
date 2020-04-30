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
  int can_adress;
  if (CANmessageID > 2304){
    can_adress = (CANmessageID - 2304)+144;
    CAN.sendMsgBuf(0x0+can_adress, 0, 8, buf);
  }else {
    if (STOpSTARt == false){
      if (CANmessageID < 60){
          if (CANmessageID > 48){
            can_adress = (CANmessageID - 48)+768;
            CAN.sendMsgBuf(0x00000+can_adress, 1, 4, buf);
          } else if (CANmessageID > 32) {
              can_adress = (CANmessageID - 32)+512;
              CAN.sendMsgBuf(0x00000+can_adress, 1, 4, buf);
            } else if (CANmessageID > 16) {
                  can_adress = (CANmessageID - 16)+256;
                  CAN.sendMsgBuf(0x00000+can_adress, 1, 4, buf);
              }
      }    
    }
  }
  ClearBuffer();
}
/*--------------------------- Stop Motors------------------------------------------------------------------------------------------------------*/

void STOP(){
  for (int i=0;i<4;i++){
     CAN.sendMsgBuf(0x00000200+CANvescID[i], 1, 4, currentREVERS); // motorum se pošle brzdí proud
     CAN.sendMsgBuf(0x0020+CANvescID[i], 0, 4, currentREVERS);
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

