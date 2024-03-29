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
  CAN.init_Mask(0, 1, 0x1fffffff); //CAN.init_Mask(0, 1, 0x00000600)
  CAN.init_Mask(1, 1, 0x1fffffff); //CAN.init_Mask(1, 1, 0x00000600)
}

/*-----------------------CAN filters initialization--------------------------------*/
void CAN_filters_initial(){
  CAN.init_Filt(0, 1, 0x504);
  CAN.init_Filt(1, 1, 0x504);
  CAN.init_Filt(2, 1, 0x501);
  CAN.init_Filt(3, 1, 0x502);
  CAN.init_Filt(4, 1, 0x503);
  CAN.init_Filt(5, 1, 0x504);
}

/*--------------------------- Přijatá zpráva CAN----------------------------------------------------------------------------------*/

void CAN_MESSAGE_ReceiveD(int vesc_id){
  
  sending_tacho_request(vesc_id); // sending request to vescs
  can_message_processing(vesc_id);  
}

/*--------------------------- tacho buffer clear----------------------------------------------------------------------------------*/

void tacho_buffer_clear(){

  for (int i=0;i<8;i++){
    tacho_buf[i] = 0xff;  
  }
}

/*-------------------------- Probuzeni arduina od interniho preruseni ----------------*/

ISR(TIMER1_OVF_vect)  {
  TCNT1 = preload_timer;
  Timer3Over = true;
}

/*-------------------------- CAN_error_chacking-------------------------*/

void CAN_error_chacking () {
  
  if (CAN.checkError() == CAN_CTRLERROR) {
    CAN_error= 1;
  } else {
    CAN_error= 0;
  }
   error_buf[1] = CAN_error;
}

/*-------------------------- can message processing -------------------------*/

void can_message_processing(int vesc_id) {
   int i;   
   
   for (i=0; i<20;i++){ 
      while (CAN.checkReceive() != CAN_MSGAVAIL){
          if (Timer3Over == true){
             return;
          }
      }
      CAN.readMsgBuf(&len, buf);
      CANmessageID = CAN.getCanId();
      //CAN_error_chacking ();
      
      if (buf[0]==7 && CANmessageID ==(CANvescID[vesc_id]+1280)){  
        create_tacho_buf_apu(vesc_id);
        CANmessageID = 0;
        break;
     }      
    }
}

/*-------------------------- Sending request via CAN to vesc for tacho -------------------------*/

void sending_tacho_request(int vesc_id) {
  int can_id = 2048;
  
  request_buf[0] = CANvescID[vesc_id]; // jeden vesc4x --- CANvescID[0]
  CAN.sendMsgBuf(0x00000+can_id+CANvescID[vesc_id], 1, 7, request_buf); // jeden vesc 4x nahradit ----- CAN.sendMsgBuf(0x00000+can_id+CANvescID[0], 1, 3, request_buf);
}

/*-------------------------- create tacho_buf to apu -------------------------*/

void create_tacho_buf_apu(int vesc_id) {
  int i = 7;
  int j;
  int buf_position = tacho_bufer_index_tabel[vesc_id];
  
  buf[7] = buf[4];
  buf[6] = buf[3];
  
  for (j=0;j<2;j++){
    i=i-j;
    tacho_buf[buf_position - j] =buf[i];
  }
}

/*---------------------------- counter_cycle--------------------------------------------------------------------------------------------------*/

void counter_cycle(){

  error_buf[0]= counter;
  if (counter == 255){
    counter = 1;
  } else {
     counter = counter+1;
    }
  
}

/*-------------------------- initialize timer1 ------------------------------------------*/

void timer1_init() {
  noInterrupts();
  
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = preload_timer;            // preload timer 65536-16MHz/256/20Hz
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}
