/*---------------------- Mereni proudu 20A sondou ------------------------------------*/

float proud20A (byte pin) {

  int NamerenyProud;  // vstup z proudove sondy
  float CurrentSouc;  // pomocna promena pro vypocet prumeru
  float Current;  // vysledny proud

  CurrentSouc = 0;
  
  for (int i = 0; i < 10; i++) {
    NamerenyProud = analogRead(pin);
    CurrentSouc = CurrentSouc + NamerenyProud;
  }

  CurrentSouc = CurrentSouc / 10;
 
  Current = CurrentSouc * (5.0 / 1024.0);
  Current = Current - (2.5); // Pro 0 A sonda vraci 2.5 V
  Current = Current * 10.0; // Protoze je 100 mV/A

  Serial.print(Current);
  return abs(Current);
}

/*-------------------------- SyrenaDown ------------------------------------------------------------------------------*/

void BeepDown () {
  
  Timer1.stop();
  Timmer1End = true;
}

/*-------------------------- SyrenaUP ------------------------------------------------------------------------------*/

void BeepUP () {
  
  digitalWrite(led, HIGH);
  Timer1.initialize(300000);
}

/*------------------------ timer1 - overflow -----------------------------------------------------*/

void timer_overflow(){
  timmer_flag = true;
}

/*----------------------- CAN Message current ------------------------------------------------------*/

void current_message(){
  int current = celk_proud * 1000; // proud v mA
  float pom;
  int ground = 256;

  for (int i=0;i<3;i++){
    buf_current[i]= current / power(ground,(2-i));
    current = current - (buf_current[i] * power(ground,(2-i)));
    //Serial.print(power(ground,(2-i)));
  }
}

/*----------------------- CAN Message resistance ------------------------------------------------------*/

void resistance_message(){
  int resist = resist_potnciometr;
  int ground = 256;

  for (int i=0;i<4;i++){
    buf_resistance[i]= resist / power(ground,(3-i));
    resist = resist - (buf_resistance[i] * power(ground,(3-i)));
    Serial.print(" ");
    Serial.print(power(ground,(3-i)));
    Serial.print(" ");
    Serial.print(buf_resistance[i]);
  }
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

/*-------------------- potenciometr -----------------------------------------------------------------*/

unsigned int potenciomter (int pin) {
unsigned int resistance = 0;
  
  for (int i=0;i<16;i++) {
    resistance = resistance + analogRead(pin);
  }

  Serial.print(" ");
  Serial.println(resistance);
  return(resistance);
}

