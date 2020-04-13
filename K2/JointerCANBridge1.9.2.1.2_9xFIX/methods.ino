/*-----------------------Serial initialization--------------------------------*/
void serial_initial(){
  Serial.begin(250000); 
  while (!Serial) {
    ; 
    }
}

/*-----------------------pin mode initialization--------------------------------*/
void pin_mode_initial(){
  pinMode(ledPin, OUTPUT);
  pinMode(Syrena,OUTPUT); //buzzer
  pinMode(stop_led, OUTPUT);
  pinMode(break_led, OUTPUT);

  pinMode(StopButton,INPUT_PULLUP);
  pinMode(control_break,INPUT_PULLUP);
}

/*-----------------------pin out setting--------------------------------*/
void pin_out_setting(){
  digitalWrite(ledPin, LOW);
  digitalWrite(Syrena, LOW);
}

/*-------------------------- initialize timer3 and 4 ------------------------------------------*/
void timer3_4_initial(){
  Timer3.attachInterrupt(TimerInterrupt);
  Timer4.attachInterrupt(BeepDown);
}

/*-----------------------wdt initial--------------------------------*/
void wdt_initial(){
  // Enable watchdog.
  WDT->WDT_MR = WDT_MR_WDD(0xFF) // velikost musí pokrýt hodnotě v WDT_MR_WDV(xx)
                | WDT_MR_WDRPROC
                | WDT_MR_WDRSTEN
                | WDT_MR_WDV(33); //set wdt to 0,12890625‬ S
                /*| WDT_MR_WDV(256 * 2); // Watchdog triggers a reset after 2 seconds if underflow
                                       // 2 seconds equal 84000000 * 2 = 168000000 clock cycles
   Slow clock is running at 32.768 kHz
   watchdog frequency is therefore 32768 / 128 = 256 Hz
   WDV holds the periode in 256 th of seconds  */
}

/*-----------------------wdt reset--------------------------------*/
void wdt_reset(){
  //Restart watchdog
  WDT->WDT_CR = WDT_CR_KEY(WDT_KEY)
                | WDT_CR_WDRSTT;
}

/*-----------------------CAN initialization--------------------------------*/
void CAN_initial(){
  Serial.println("Setup can..");
  while(CAN.begin(CAN_500KBPS, MCP_8MHz) != CAN_OK){
    digitalWrite(stop_led, HIGH);
    Serial.print(".");
    BeepUP();
    delay(1000);
  }
  Serial.print("\nCAN init ok!!\r\n");

  for (int i=0;i<3;i++){
    CAN_Mask_initial();
    CAN_filters_initial();
  }
}

/*-----------------------waiting for timer4 overflow--------------------------------*/
void waiting_for_timer4_overflow(){
  while(Timmer4End != true){
    delay(100);
  }
}

/*-----------------------CAN Mask initialization--------------------------------*/
void CAN_Mask_initial(){
  CAN.init_Mask(0, 1, 0x1FFFFFF0); // 29 bitů  extended adresa
  CAN.init_Mask(1, 0, 0x3f0); 
}

/*-----------------------CAN filters initialization--------------------------------*/
void CAN_filters_initial(){
  CAN.init_Filt(0, 1, 0x900); 
  CAN.init_Filt(1, 1, 0x900);
  CAN.init_Filt(2, 0, 0x10);
  CAN.init_Filt(3, 0, 0x20);
  CAN.init_Filt(4, 0, 0x30);
  CAN.init_Filt(5, 0, 0x30);
}
/*-----------------------CANmessage print to serial--------------------------------*/
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

/*-------------------------- Timer3 overflow ----------------*/

void TimerInterrupt () {
  Timer3Over = true;
}

/*---------------------------Led indication ----------------------------------------------------------------*/
void LEDIndicator () {
  for (int i=0; i <= 3; i++) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
  }
  digitalWrite(stop_led, LOW);
  digitalWrite(ledPin, HIGH);
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
  digitalWrite(ledPin, HIGH);
  Timmer4End = true;
}

/*-------------------------- SyrenaUP ------------------------------------------------------------------------------*/

void BeepUP () {
  
  digitalWrite(Syrena, HIGH);
  digitalWrite(ledPin, LOW);
  Timer4.start(300000);
}

/*---------------------------StopButton--------------------------------------------------------------------------------------*/

void ButtonRead(){
  digitRead = digitalRead(StopButton);
  if (digitRead==true){
    STOpSTARt = true;
    digitalWrite(stop_led, HIGH);
  } else {
    STOpSTARt = false;
    digitalWrite(stop_led, LOW);
  }
}

/*---------------------------brake Button--------------------------------------------------------------------------------------*/

void brake_button_read(){
  digit_read_brake = digitalRead(control_break);
  if (digit_read_brake==true){
   brake_control = false;
   digitalWrite(break_led, HIGH);
  } else {
    brake_control = true;
    digitalWrite(break_led, LOW);
  }
}

/*---------------------------- Send buttons status to APU --------------------------------------------------------------------------*/

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

/*--------------------------- received CAN message processing ----------------------------------------------------------------------------------*/

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
    if (brake_control==true){
     CAN.sendMsgBuf(0x00000200+CANvescID[i], 1, 4, currentREVERS); // motorum se pošle brzdí proud
     CAN.sendMsgBuf(0x0020+CANvescID[i], 0, 4, currentREVERS);
    } else {
     CAN.sendMsgBuf(0x00000100+CANvescID[i], 1, 4, current_no_brake); // vescy nebrzdí
     CAN.sendMsgBuf(0x0010+CANvescID[i], 0, 4, current_no_brake);
    }
   }
}
/*---------------------------- Cycle count --------------------------------------------------------------------------------------------------*/

void CountCycle(){
  byte bufCount[1];

  bufCount[0]= CycleCount;
  CAN.sendMsgBuf(0x02, 0, 1, bufCount);
  if (CycleCount==255){
    CycleCount = 1;
  } else {
     CycleCount = CycleCount+1;
    }
  
}
