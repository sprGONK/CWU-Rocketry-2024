/*
Circuit:
 
 SCK: pin 13
 MISO: pin 12
 MOSI: pin 11
 SS: pin 9
 */


const int ss = 5;
#include "SPI.h"

SPIClass spi(VSPI);


//3 bytes to get the 24 bit adc read
byte byte1; 
byte byte2; 
byte byte3;

uint32_t temp;


void setup() {
  Serial.begin(9600);

  spi.begin();
  spi.beginTransaction(SPISettings(10000000,MSBFIRST,SPI_MODE0));

  pinMode(ss, OUTPUT);

  //Reset MS5607
  digitalWrite(ss,LOW);
  spi.transfer(0x1E);
  delay(3);
  digitalWrite(ss,HIGH);
}


void loop() {
  if (Serial.available() > 0) {
    byte incomingByte = Serial.read();
    switch (incomingByte){
    case '0':
      read_alt();
      break;
    } //endswitch
  }//enif
}//endloop


void read_alt()
{
  //READ Altimeter
  digitalWrite(ss,LOW);
  spi.transfer(0x48); //ADC conversion command
  delay(10);
  digitalWrite(ss,HIGH);

  digitalWrite(ss, LOW);
  // and store read data into three bytes
  byte1 = spi.transfer(0x00);//ADC conversion read command, first 8 bits
  byte2 = spi.transfer(0x00);//ADC conversion read command, second 8 bits
  byte3 = spi.transfer(0x00);//ADC conversion read command, third 8 bits
  
  //release chip, signal end transfer
  digitalWrite(ss, HIGH); 

  temp = byte1*65536 + byte2*256 + byte3;
  //print the 3 bytes in serial
  //Serial.println(byte1, BIN);
  //Serial.println(byte2, BIN);
  //Serial.println(byte3, BIN);
  Serial.print(temp);
  Serial.println();
}
