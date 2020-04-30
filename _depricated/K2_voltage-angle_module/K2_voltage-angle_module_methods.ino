/*---------------------- Voltage meassure ------------------------------------*/

float Voltage (byte pin,float resist1, float resist2) {

  int meassure_voltage;  
  float voltage_count;  // pomocna promena pro vypocet prumeru
  float voltage;  // vysledny voltage

  voltage_count = 0;
  
  for (int i = 0; i < 10; i++) {
    meassure_voltage = analogRead(pin);
    voltage_count = voltage_count + meassure_voltage;
  }

  voltage_count = voltage_count / 10;
 
  voltage = voltage_count * (5.0 / 1024.0);
  voltage = (voltage / (resist2/(resist1+resist2)));
  //voltage = (voltage-(voltage *0.07)); // korekce děliču - upravit 

  Serial.println(voltage,3);
  return (voltage);
}

/*------------------------ timer1 - overflow -----------------------------------------------------*/

void timer_overflow(){
  timmer_flag = true;
}

/*----------------------- CAN Message voltage12V ------------------------------------------------------*/

void voltage_message12(){
  int voltage_pom12 = voltage12  * 1000; // proud v mV
  float pom;
  int ground = 256;

  for (int i=0;i<4;i++){
    buf_voltage12[i]= voltage_pom12 / power(ground,(3-i));
    voltage_pom12 = voltage_pom12 - (buf_voltage12[i] * power(ground,(3-i)));
  //Serial.print(power(ground,(4-i)));
    Serial.print(buf_voltage12[i]);
  }
  Serial.println("");
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
    Serial.print(buf_voltage36[i]);
  }
  Serial.println("");
}

/*----------------------- CAN Message resistance ------------------------------------------------------*/

void resistance_message(){
  int resist = resist_potnciometr;
  int ground = 256;

  for (int i=0;i<4;i++){
    buf_resistance[i]= resist / power(ground,(3-i));
    resist = resist - (buf_resistance[i] * power(ground,(3-i)));
    /*Serial.print(" ");
    Serial.print(power(ground,(3-i)));
    Serial.print(" ");
    Serial.print(buf_resistance[i]);*/
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

  /*Serial.print(" ");
  Serial.println(resistance);
  return(resistance);*/
}

