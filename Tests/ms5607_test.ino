/*
Circuit:
 
 SCK: pin 13
 MISO: pin 12
 MOSI: pin 11
 SS: pin 9
 */


const int ss = 5;
#include "SPI.h"

SPIClass SPI(VSPI);


//3 bytes to get the 24 bit adc read
byte byte1; 
byte byte2; 
byte byte3;

uint32_t temp;


void setup() {
  Serial.begin(9600);


  SPI.begin();
  SPI.setBitOrder(MSBFIRST);


  pinMode(ss, OUTPUT);
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
  digitalWrite(ss, LOW);
  SPI.transfer32(0x50); //ADC conversion command
  delay(1);


  // and store read data into three bytes
  temp = SPI.transfer32(0x00);//ADC conversion read command, first 8 bits
  byte2 = SPI.transfer16(0x00);//ADC conversion read command, second 8 bits
  byte3 = SPI.transfer16(0x00);//ADC conversion read command, third 8 bits

  //release chip, signal end transfer
  digitalWrite(ss, HIGH); 

  //print the 3 bytes in serial
  Serial.println(temp);
  Serial.println(byte2, BIN);
  Serial.println(byte3, BIN);
  Serial.println();


  
}
