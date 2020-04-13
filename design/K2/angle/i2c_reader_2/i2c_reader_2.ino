#include <Wire.h>

void setup()
{
  Serial.begin(115200);
  while (!Serial);           // wait for serial monitor
  Serial.println("Serial OK");
  
  Wire.begin();
  Wire.setClock(400000);     // use 400 kHz I2C

  Serial.println("I2C Scanner");
  i2c_scanner();
}

void loop()
{
  Wire.beginTransmission(0x42);
  
  Wire.write(254);
  Wire.requestFrom(0x42,1);
  //delay(1);
  byte byte254 = Wire.read();
  
  Wire.write(255);
  Wire.requestFrom(0x42,1);
  //delay(1);
  byte byte255 = Wire.read();
  
  Wire.endTransmission();

  /*char buf[6];
  int r = sprintf(buf, "%04d", number);
  int result = printf("%04d", byte255);
  Serial.println(buf,BIN);*/
  
  Serial.print(byte254,BIN);
  Serial.print(";"); 
  Serial.println(byte255,BIN);
  Serial.print(byte254*255);
  //Serial.print("-"); 
  Serial.println(byte255);
  /*
  Wire.requestFrom(0x42, 2);   // request 6 bytes from slave device #8

  while (Wire.available())     // slave may send less than requested
  {   
    char c = Wire.read();      // receive a byte as character
    Serial.print(c,BIN);           // print the character
    Serial.print(";");
  }
  Serial.println("");*/
  delay(50);
}
