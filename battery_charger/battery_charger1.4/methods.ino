/*----------------------Serial-initialization------------------------------------*/
void serial_initial(){
  Serial.begin(250000); 
  while (!Serial) {
      ; 
  }
}

/*----------------------pinouts-initialization------------------------------------*/
void pinouts_initial(){
  pinMode (relay_pin,OUTPUT);
  digitalWrite(relay_pin,LOW);
  delay(10);
  
  pinMode (buzzer_pin,OUTPUT);
  buzzer_beep();
}

/*----------------------buzzer_beep------------------------------------*/
void buzzer_beep(){
  for(int i=0;i<2;i++){
    digitalWrite(buzzer_pin,HIGH);
    delay(250);
    digitalWrite(buzzer_pin,LOW);
    delay(250);
  }
}

/*-------------------------- initialize timer1 ------------------------------------------*/

void timer_init() {
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(timer1_oveflow); // blinkLED to run every 1.0 seconds
}

/*-------------------------- timer1 overflow ----------------*/

void timer1_oveflow (void) {
  wdt_reset (); // Reset watchdog counter
  voltage = voltage_meassure (voltage_pin,r1,r2);
  current = current_meassure (current_pin);
  charging();
}

/*---------------------- Voltage meassure ------------------------------------*/

float voltage_meassure(byte pin,float resist1, float resist2) {

  int meassure_voltage;  
  float voltage_count;  // pomocna promena pro vypocet prumeru
  float voltage;  // vysledny voltage

  voltage_count = 0;
  
  for (int i = 0; i < 200; i++) {
    meassure_voltage = analogRead(pin);
    voltage_count = voltage_count + meassure_voltage;
  }

  voltage_count = voltage_count / 200;
 
  voltage = voltage_count * (5.0 / 1024.0);
  voltage = (voltage / (resist2/(resist1+resist2)));
  
  if (voltage < 25.0) {
    voltage = voltage * 1.0; //calibration
  } else if (voltage < 28.0) {
    voltage = voltage * 0.98452012; //calibration
  } else if (voltage < 38.0){
    voltage = voltage * 0.98419244; //calibration
  } else if (voltage < 43.0){
    voltage = voltage * 0.98255515; //calibration 
  }
  
  return (voltage);
}

/*----------------------  current meassure 20 A probe ------------------------------------*/

float current_meassure (byte pin) {

  float current_sum;
  float current;  // vysledny proud

  current_sum = 0;
  
  for (int i = 1; i <= 10; i++) {
    current_sum = current_sum + analogRead(pin);
  }

  current_sum = current_sum / 10;
 
  current = current_sum * (5.0 / 1024.0);
  current = current - 2.5; // 0 A is 2.5 V
  current = current * 10.0; // 100 mV/A
  
  return current;
}

/*----------------------  write to serial ------------------------------------*/

void write_to_serial() {
  Serial.print("volatge : ");
  Serial.print(voltage,3);
  Serial. println(" V");
  Serial.print("current : ");
  Serial.print(current,3);
  Serial. println(" A");
}

/*----------------------  characters table for display ------------------------------------*/

void characters_table_for_display() {
  LED_0F[0] = 0xC0; //0
  LED_0F[1] = 0xF9; //1
  LED_0F[2] = 0xA4; //2
  LED_0F[3] = 0xB0; //3
  LED_0F[4] = 0x99; //4
  LED_0F[5] = 0x92; //5
  LED_0F[6] = 0x82; //6
  LED_0F[7] = 0xF8; //7
  LED_0F[8] = 0x80; //8
  LED_0F[9] = 0x90; //9
  LED_0F[10] = 0x88; //A
  LED_0F[11] = 0x83; //b
  LED_0F[12] = 0xC6; //C
  LED_0F[13] = 0xA1; //d
  LED_0F[14] = 0x86; //E
  LED_0F[15] = 0x8E; //F
  LED_0F[16] = 0xC2; //G
  LED_0F[17] = 0x89; //H
  LED_0F[18] = 0xF9; //I
  LED_0F[19] = 0xF1; //J
  LED_0F[20] = 0xC3; //L
  LED_0F[21] = 0xA9; //n
  LED_0F[22] = 0xC0; //O
  LED_0F[23] = 0x8C; //P
  LED_0F[24] = 0x98; //q
  LED_0F[25] = 0x92; //S
  LED_0F[26] = 0xC1; //U
  LED_0F[27] = 0x91; //Y
  LED_0F[28] = 0xFE; //hight 
}

/*---------------------- display initialization ------------------------------------*/

void display_init() {
  disp.send(LED_0F[0], 0b1111);    //send digital "0" to 1st indicator
  delay(2000);
}

/*---------------------- display voltage ------------------------------------*/

void display_voltage() {
  int volatge_display;
  volatge_display = voltage * 10;
  for (int i=0;i<100;i++){
    disp.send(LED_0F[26], 0b1000);
    delay(5);
    disp.digit4(volatge_display, 20); //send counter 0-9999 with delay 50 cicles and hide zero
  }
}

/*---------------------- display current ------------------------------------*/

void display_current() {
  int current_display;
  current_display = current * 10;
  for (int i=0;i<100;i++){
    disp.send(LED_0F[10], 0b1000);
    delay(5);
    disp.digit4(current_display, 20); //send counter 0-9999 with delay 50 cicles and hide zero
  }
}

/*---------------------- charging ------------------------------------*/

void charging() {
  if ((voltage < voltage_max_limit)&&(charged == false)){
    digitalWrite(relay_pin,HIGH);
    delay(10);
  } else {
     charged = true;
  }
  if ((charged == true)&&(current <= current_cutoff_limit )) {
    digitalWrite(relay_pin,LOW);
    delay(10);
    buzzer_beep();
}
}
