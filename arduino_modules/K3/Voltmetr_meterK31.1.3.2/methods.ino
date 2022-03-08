/*----------------------I2C-initialization------------------------------------*/
void I2C_initial(){
  Wire.begin();
  Wire.setClock(100000);     // use 100 kHz I2C
}
/*----------------------I2C-scanner------------------------------------*/
void I2C_scanner()
{
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  for (byte i = 1; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1); 
    }
  }
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}

/*----------------------Serial-initialization------------------------------------*/
void serial_initial(){
  Serial.begin(250000); 
  while (!Serial) {
      ; 
  }
}

/*----------------------CAN-initialization------------------------------------*/
void CAN_initial(){
  Serial.println("Setup can..");
  while(CAN.begin(CAN_500KBPS, MCP_8MHz) != CAN_OK){
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nCAN init ok!!\r\n");
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

/*-------------------------- Probuzeni arduina od interniho preruseni ----------------*/

ISR(TIMER1_OVF_vect)  {
  TCNT1 = preload_timer;
  timmer_flag = true;
}

/*---------------------- Voltage meassure ------------------------------------*/

float Voltage (byte pin,float resist1, float resist2) {

  int meassure_voltage;  
  float voltage_count;  // pomocna promena pro vypocet prumeru
  float voltage;  // vysledny voltage

  voltage_count = 0;
  
  for (int i = 0; i < 3; i++) {
    meassure_voltage = analogRead(pin);
    voltage_count = voltage_count + meassure_voltage;
  }

  voltage_count = voltage_count / 3;
 
  voltage = voltage_count * (5.0 / 1024.0);
  voltage = (voltage / (resist2/(resist1+resist2)));
  if (pin == A1) {
    voltage = voltage * 0.982; // corection of 42 V voltage
  }

  //Serial.println(voltage,3);
  return (voltage);
}

/*----------------------- CAN Message voltage12V ------------------------------------------------------*/

void voltage_message12(){
  int voltage_pom12 = voltage12  * 1000; // voltage in mV
  float pom;
  int ground = 256;
  
  for (int i=0;i<4;i++){
    buf_voltage12[i]= voltage_pom12 / power(ground,(3-i));
    voltage_pom12 = voltage_pom12 - (buf_voltage12[i] * power(ground,(3-i)));
    //Serial.print(power(ground,(4-i)));
    //Serial.print(buf_voltage12[i]);
  }
 //Serial.println("");
}

/*----------------------- CAN Message voltage36V ------------------------------------------------------*/

void voltage_message36(){
  int voltage_pom36 = voltage36  * 1000; // proud v mV
  float pom;
  int ground = 256;

  for (int i=0;i<4;i++){
    buf_voltage36[i]= voltage_pom36 / power(ground,(3-i));
    voltage_pom36 = voltage_pom36 - (buf_voltage36[i] * power(ground,(3-i)));
   //Serial.print(power(ground,(4-i)));
   //Serial.print(buf_voltage36[i]);
  }
  //Serial.println("");
}

/*--------------------- power ----------------------------------------------------------------------*/

float power (int ground ,int exponent) {
  float compute = 0;
  if (exponent > 1) {
    compute = ground;
    for (int i=0; i<(exponent-1);i++) {
      compute = compute * ground;
    }
   } else if (exponent == 0) {
        compute = 1;
     } else {
          compute = ground;
       }
  return(compute);
}

/*-------------------- angle measure -----------------------------------------------------------------*/

void angle_measure (byte adrress) {
  
  Wire.beginTransmission(adrress);
  
  Wire.write(254);
  Wire.requestFrom(adrress,1); //adrress i2c
  angle_254 = Wire.read();
  
  Wire.write(255);
  Wire.requestFrom(adrress,1);
  angle_255 = Wire.read();
  
  Wire.endTransmission();
}

/*-------------------- angles measurement -----------------------------------------------------------------*/

void angles_measurement () {

  angle_measure(sensor1_i2c_adrress);
  buf_angles[0] = angle_254;
  buf_angles[1] = angle_255;

  angle_measure(sensor2_i2c_adrress);
  buf_angles[2] = angle_254;
  buf_angles[3] = angle_255;
  
}

/*-------------------- voltage measurement -----------------------------------------------------------------*/

void voltage_measurement (){
  voltage12 =  Voltage (voltege12_pin,R1_12, R2_12);
  voltage36 =  Voltage (voltege36_pin,R1_36, R2_36);
  voltage_message12();
  voltage_message36();
}

/*-------------------- CAN messages sending -----------------------------------------------------------------*/

void CAN_messages_sending () {
  delayMicroseconds(20);
  CAN.sendMsgBuf(0x00 + can_adress_voltage12, 0, 4, buf_voltage12);
  delayMicroseconds(20);
  CAN.sendMsgBuf(0x00 + can_adress_voltage36, 0, 4, buf_voltage36);
  delayMicroseconds(20);
  CAN.sendMsgBuf(0x00 + can_adress_angles, 0, 4, buf_angles);

  variables_clearing ();
}

/*-------------------- variables clearing -----------------------------------------------------------------*/

void variables_clearing () {
  for (int i = 0; i < 4; i++) {
    buf_voltage12[i] = 0;
    buf_voltage36[i] = 0;
    buf_angles[i] = 0;
  }
  voltage12 = 0.0;
  voltage36 = 0.0;
}
