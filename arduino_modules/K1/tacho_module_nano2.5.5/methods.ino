/*--------------------------- Přijatá zpráva CAN----------------------------------------------------------------------------------*/

void CAN_MESSAGE_ReceiveD(int vesc_id){
  sending_tacho_request(vesc_id); // sending request to vescs
  can_message_processing(vesc_id);  
}
/*-------------------------- Probuzeni arduina od interniho preruseni ----------------*/

ISR(TIMER1_OVF_vect)  {
  TCNT1 = preload_timer;
  Timer3Over = true;
}

/*-------------------------- can message processing -------------------------*/

void can_message_processing(int vesc_id) {
  int i;
  int tacho;
  
    for (i=0; i<10;i++) {
      while((CAN.readMsgBuf(&len, buf))==CAN_NOMSG){
        if (Timer3Over == true){
          break;
        }
      }
      if (buf[0]==42 && CAN.getCanId()==(CANvescID[vesc_id]+1280)){  
        create_tacho_buf_apu(vesc_id);
      }
      if (len < 8 && CAN.getCanId()==(CANvescID[vesc_id]+1280)){  
        break;
      }
      if (Timer3Over == true){
          break;
      }
    }  
}

/*-------------------------- Sending request via CAN to vesc for tacho -------------------------*/

void sending_tacho_request(int vesc_id) {
  int can_id = 2048;
  
  request_buf[0] = CANvescID[vesc_id]; // jeden vesc4x --- CANvescID[0]
  //Serial.println(request_buf[0]);
  CAN.sendMsgBuf(0x00000+can_id+CANvescID[vesc_id], 1, 3, request_buf); // jeden vesc 4x nahradit ----- CAN.sendMsgBuf(0x00000+can_id+CANvescID[0], 1, 3, request_buf);
}

/*-------------------------- create tacho_buf to apu -------------------------*/

void create_tacho_buf_apu(int vesc_id) {
  int i = 7;
  int j;
  int buf_position = tacho_bufer_index_tabel[vesc_id];
  
  for (j=0;j<2;j++){
    i=i-j;
    //Serial.print(buf_position - j);
    tacho_buf[buf_position - j] =buf[i];
  }
  //Serial.println("");
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

