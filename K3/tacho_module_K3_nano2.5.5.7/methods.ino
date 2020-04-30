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
  CAN.init_Filt(0, 1, 0x501);
  CAN.init_Filt(1, 1, 0x502);
  CAN.init_Filt(2, 1, 0x503);
  CAN.init_Filt(3, 1, 0x504);
  CAN.init_Filt(4, 1, 0x505);
  CAN.init_Filt(5, 1, 0x506);
}

/*--------------------------- Tacho service----------------------------------------------------------------------------------*/

void tacho_service(int vesc_id){
  sending_tacho_request(vesc_id); // sending request to vescs
  can_message_processing(vesc_id);  
}

/*--------------------------- cycle counter service----------------------------------------------------------------------------------*/

void cycle_count_service(){
  if (cycle_count == 255){
      cycle_count = 0;
    }
  cycle_count = cycle_count + 1;
}

/*-------------------------- Timer1 overflow ----------------*/

ISR(TIMER1_OVF_vect)  {
  TCNT1 = preload_timer;
  Timer1Over = true;
}

/*-------------------------- can message processing -------------------------*/

void can_message_processing(int vesc_id) {
  int i;
  int tacho;
  
    for (i=0; i<10;i++) {
      while((CAN.readMsgBuf(&len, buf))==CAN_NOMSG){
        if (Timer1Over == true){
          break;
        }
      }
      if (buf[0]==42 && CAN.getCanId()==(CANvescID[vesc_id]+1280)){  
        create_tacho_buf_apu(vesc_id);
      }
      if (len < 8 && CAN.getCanId()==(CANvescID[vesc_id]+1280)){  
        break;
      }
      if (Timer1Over == true){
          break;
      }
    }  
}

/*-------------------------- tacho_buf_clear -------------------------*/

void buffers_clear() {
  for(int i=0;i<tacho_message_lenth;i++){
    tacho_buf[i] = 0;
  }
  buf[6] = 0;
  buf[7] = 0;
}

/*-------------------------- Sending request via CAN to vesc for tacho -------------------------*/

void sending_tacho_request(int vesc_id) {
  int can_id = 2048;
  
  request_buf[0] = CANvescID[vesc_id]; // one vesc4x --- CANvescID[0]
  CAN.sendMsgBuf(0x00000+can_id+CANvescID[vesc_id], 1, 3, request_buf); // one vesc 6x
}

/*-------------------------- Sending CAN message to APU -------------------------*/

void sending_tacho_apu(int vesc_id) {
 delayMicroseconds(20);
 if ((CANvescID[vesc_id] & 1) == 0) {
    CAN.sendMsgBuf(0x0+can_adress_transmit, 0, 6, tacho_buf);  // sending tacho infos to APU
    buffers_clear();
 }
}

/*-------------------------- create tacho_buf to apu -------------------------*/

void create_tacho_buf_apu(int vesc_id) {
  
  tacho_buf[0] = cycle_count;
  if (CANvescID[vesc_id] < 3) {
    tacho_buf[1] = 1; // index of vescs pair
    if (CANvescID[vesc_id]== 1) {
      tacho_buf[2] = buf[6];   // left motor tacho info
      tacho_buf[3] = buf[7];   // left motor tacho info
    } else if (CANvescID[vesc_id] == 2) {
         tacho_buf[4] = buf[6];   // right motor tacho info
         tacho_buf[5] = buf[7];   // right motor tacho info
      }
  } else if (CANvescID[vesc_id] < 5) {
      tacho_buf[1] = 2; // index of vescs pair
      if (CANvescID[vesc_id] == 3) {
        tacho_buf[2] = buf[6];   // left motor tacho info
        tacho_buf[3] = buf[7];   // left motor tacho info
      } else if (CANvescID[vesc_id] == 4) {
           tacho_buf[4] = buf[6];   // right motor tacho info
           tacho_buf[5] = buf[7];   // right motor tacho info
        }
  } else {
      tacho_buf[1] = 3; // index of vescs pair
      if (CANvescID[vesc_id] == 5) {
        tacho_buf[2] = buf[6];   // left motor tacho info
        tacho_buf[3] = buf[7];   // left motor tacho info
      } else if(CANvescID[vesc_id] == 6) {
           tacho_buf[4] = buf[6];   // right motor tacho info
           tacho_buf[5] = buf[7];   // right motor tacho info
        }
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
