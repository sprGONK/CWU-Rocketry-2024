// We'll use HardwareSerial to communicate with the XBee:
#include <HardwareSerial.h>
#include "Adafruit_MPL3115A2.h"
#define RX 16
#define TX 17
#define LED 2

HardwareSerial XBee(2); // UART2
Adafruit_MPL3115A2 baro;

int getCurrentAltitude();

float baseAltitude;

void setup()
{
  // Set up both ports at 9600 baud. This value is most important
  // for the XBee. Make sure the baud rate matches the config
  // setting of your XBee.
  XBee.begin(9600,SERIAL_8N1,RX,TX);
  Serial.begin(9600);
  pinMode(LED,OUTPUT);

  //setup altimeter
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Adafruit_MPL3115A2 test!");

  if (!baro.begin()) {
    Serial.println("Could not find sensor. Check wiring.");
    while(1){
      Serial.println("FUck you");
    }
  }
  // use to set sea level pressure for current location
  // this is needed for accurate altitude measurement
  // STD SLP = 1013.26 hPa
  // Note: 1 hPa = 1 mbar
  // Ellensburg = 1020 hPa Pasco = 1021 hPa
  baro.setSeaPressure(1019.0);
  baseAltitude = baro.getAltitude()*3.28;
} 
 
void loop()
{
  if (XBee.available() > 0) {
    byte incomingByte = XBee.read();
    switch (incomingByte){
    case '0x31':
        digitalWrite(LED,!digitalRead(LED));
        break;
    case '0x32':
        XBee.write(getCurrentAltitude());
      break;
    } //endswitch
  }//enif
}

int getCurrentAltitude(){
  return baro.getAltitude()*3.28 - baseAltitude;
}