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

/*-----------------------Pinout-initialization--------------------------------*/
void pinout_initial(){
  for (int i=0; i<led_pinout_count ;i++) {
    pinMode(led_pinout[i], OUTPUT);
    digitalWrite(led_pinout[i],LOW);
  }
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin,LOW);
}

/*-------------------------- initialize timer1 ------------------------------------------*/
void timer1_init() {
  Timer1.initialize(50000); // Calls every 50 mikros (20 Hz)
  Timer1.attachInterrupt(timer1_interrupt);
}

/*-------------------------- Timer1 overflow ----------------*/
void timer1_interrupt () {
  timer1_over = true;
}

/*-------------------------- buzzer ------------------------------------------------------------------------------*/
void beep () {
  
  digitalWrite(buzzer_pin, HIGH);
  delay(500);
  digitalWrite(buzzer_pin, LOW);
}

/*-----------------------Knight Rider led test--------------------------------*/
void knight_rider_led_test (){

  for (int j = 0; j<5; j++) {
    for (int i=0; i<led_pinout_count ;i++) {
      digitalWrite(led_pinout[i],HIGH);
      delay(50);
      if (i < (led_pinout_count-1)){
       digitalWrite(led_pinout[i],LOW); 
      }
    }
    for (int i=led_pinout_count; i>=0;i=i-1) {
        digitalWrite(led_pinout[i],HIGH);
        delay(50);
        digitalWrite(led_pinout[i],LOW);
      }
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
        leds_reguest_on = true;
      }
    }
  }
}

/*--------------------------- leds on ----------------------------------------------------------------------------------*/
void leds_on() {
  for (int i=0; i<led_pinout_count ;i++) {
     digitalWrite(led_pinout[i],HIGH);
  }
  led_timer_init ();
}

/*--------------------------- led timer init ----------------------------------------------------------------------------------*/
void led_timer_init() {
  led_timer_start = true;
  led_timer_count = 0;
}


/*--------------------------- led_timer_check status----------------------------------------------------------------------------------*/
void led_timer_check_status() {
  if(led_timer_count == leds_time_on) {
    led_timer_count = 0;
    leds_reguest_off == true;
    led_timer_start = false;
  }
  led_timer_count = led_timer_count + 1;
}

/*--------------------------- leds off ----------------------------------------------------------------------------------*/
void leds_off() {
  for (int i=0; i<led_pinout_count ;i++) {
     digitalWrite(led_pinout[i],LOW);
  }
}

/*--------------------------- cycle counter service----------------------------------------------------------------------------------*/
void cycle_count_service(){
  if (cycle_count == 255){
      cycle_count = 0;
    }
    
  cycle_count = cycle_count + 1;
}
