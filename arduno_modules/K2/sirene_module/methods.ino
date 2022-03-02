/*-----------------------Serial initialization--------------------------------*/
void serial_initial(){
  Serial.begin(250000); 
  while (!Serial) {
    ; 
    }
}

/*-----------------------CAN initialization--------------------------------*/
void CAN_initial(){
  Serial.println("Setup can..");
  while(CAN.begin(CAN_500KBPS, MCP_8MHz) != CAN_OK){
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nCAN init ok!!\r\n");

  for (int i=0;i<3;i++){
    CAN_Mask_initial();
    CAN_filters_initial();
  }
}

/*-----------------------CAN Mask initialization--------------------------------*/
void CAN_Mask_initial(){
  CAN.init_Mask(0, 0, 0x7ff); //CAN.init_Mask 0
  CAN.init_Mask(1, 0, 0x7ff); //CAN.init_Mask 1 
}

/*-----------------------CAN filters initialization--------------------------------*/
void CAN_filters_initial(){
  CAN.init_Filt(0, 0, 0x60);
  CAN.init_Filt(1, 0, 0x60);
  CAN.init_Filt(2, 0, 0x60);
  CAN.init_Filt(3, 0, 0x60);
  CAN.init_Filt(4, 0, 0x60);
  CAN.init_Filt(5, 0, 0x60);
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

/*-----------------------pin mode initialization--------------------------------*/
void pin_mode_initial(){
  pinMode(led_pin, OUTPUT);
  pinMode(buzzer_pin,OUTPUT); //buzzer
  pinMode(load_pin_1,OUTPUT);
  pinMode(load_pin_2,OUTPUT);
  pinMode(load_pin_3,OUTPUT);
}

/*-----------------------pin out setting--------------------------------*/
void pin_out_setting(){
  digitalWrite(led_pin, LOW);
  digitalWrite(buzzer_pin, LOW);
  digitalWrite(load_pin_1, LOW);
  digitalWrite(load_pin_2, LOW);
  digitalWrite(load_pin_3, LOW);
}

/*-------------------------- initialize timer3 and 4 ------------------------------------------*/
void timer3_4_initial(){
  Timer3.attachInterrupt(timer3_interrupt);
  Timer4.attachInterrupt(beep_down);
}

/*-------------------------- Timer3 overflow ----------------*/

void timer3_interrupt () {
  timer3_over = true;
}

/*-------------------------- buzzer off ------------------------------------------------------------------------------*/

void beep_down () {
  
  Timer4.stop();
  digitalWrite(buzzer_pin, LOW);
  digitalWrite(led_pin, HIGH);
  timmer4_end = true;
}

/*-------------------------- buzzer on ------------------------------------------------------------------------------*/

void beep_up () {
  
  digitalWrite(buzzer_pin, HIGH);
  digitalWrite(led_pin, LOW);
  Timer4.start(300000);
}
/*---------------------------Led indication ----------------------------------------------------------------*/
void led_indicator () {
  for (int i=0; i <= 3; i++) {
      digitalWrite(led_pin, HIGH);
      delay(200);
      digitalWrite(led_pin, LOW);
      delay(200);
  }
}

/*-----------------------waiting for timer4 overflow--------------------------------*/
void waiting_for_timer4_overflow(){
  while(timmer4_end != true){
    delay(100);
  }
}

/*-------------------------- Sending CAN message to APU -------------------------*/

void sending_hb_apu() {

   transmit_buf[0]= cycle_count;
   CAN.sendMsgBuf(0x0+can_adress_transmit, 0, 1, transmit_buf);  // sending heart beat to APU
 }

/*-------------------------- checkout incoming can message from APU -------------------------*/
void checkout_can_incoming_message() {

    if(CAN.readMsgBuf(&len, buf)== CAN_OK) {  
      CANmessageID = CAN.getCanId();
      can_message_processing();
  }
}

/*--------------------------- received CAN message processing ----------------------------------------------------------------------------------*/

void can_message_processing(){
  if (CANmessageID == can_adress_receive){
    if (len == len_incaming_message){
      if (buf[0]==1){
        sirene_reguest_on = true;
      }
    }
  }
}

/*--------------------------- sirene on ----------------------------------------------------------------------------------*/
void sirene_on() {
  digitalWrite(load_pin_1,HIGH);
  Serial.println("on");
  Timer5.attachInterrupt(sirene_timer_overf);
  Timer5.start(3000000);
}

/*--------------------------- sirene_timer_overf ----------------------------------------------------------------------------------*/
void sirene_timer_overf() {
  Timer5.stop();
  sirene_reguest_off = true;
}

/*--------------------------- sirene off ----------------------------------------------------------------------------------*/
void sirene_off() {
  digitalWrite(load_pin_1,LOW);
}

/*--------------------------- cycle counter service----------------------------------------------------------------------------------*/
void cycle_count_service(){
  if (cycle_count == 255){
      cycle_count = 0;
    }
  cycle_count = cycle_count + 1;
}
