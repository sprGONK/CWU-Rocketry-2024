// We'll use HardwareSerial to communicate with the XBee:
#include "HardwareSerial.h"
#include "Adafruit_MPL3115A2.h"
#define RX 16
#define TX 17
#define LED 2
#define ematch 15

HardwareSerial XBee(2); // UART2
Adafruit_MPL3115A2 baro; // altimeter
/* ignite global permission, has 4 checks
  Primer(ascent), Range, and Permission
  Primer: checking if rocket is above 1000ft
  Range: checks if we're in ejection range of 400-550ft only after Primer has happened
  Permission: checks if we've gotten ground permission
  */
int ignite,baseAltitude;

void IRAM_ATTR isr();

void setup()
{
  //setup uart for xbee
  XBee.begin(9600,SERIAL_8N1,RX,TX);
  attachInterrupt(RX,isr,CHANGE);
  //setup GPIO for ematch
  pinMode(LED,OUTPUT);
  pinMode(ematch,OUTPUT);
  //setup altimeter
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Adafruit_MPL3115A2 test!");

  if (!baro.begin()) {
    Serial.println("Could not find sensor. Check wiring.");
    while(1);
  }
  
  // use to set sea level pressure for current location
  // this is needed for accurate altitude measurement
  // STD SLP = 1013.26 hPa
  // Note: 1 hPa = 1 mbar
  // Ellensburg = 1020 hPa
  // Pasco = 1021 hPa
  baro.setSeaPressure(1013.26);
  baseAltitude = baro.getAltitude()*3.28;
}

float getCurrentAltitude(){
  return baro.getAltitude()*3.28 - baseAltitude;
}

void loop(){
  float pressure = baro.getPressure();
  float altitude = getCurrentAltitude();
  float temperature = baro.getTemperature();

  if(altitude > 1000){
    ignite |= 4; //above 1000ft? set bit 3 high
  }
  if(ignite >= 4){ // are bits 1 and or 3 high? proceed
    if((altitude < 550) && (altitude > 400)){
      ignite |= 2; // in ejection altitude range? set bit 2 high
    }
  }
  
  if(ignite == 7){ //are all 3 bits high? ignite the ematch
    digitalWrite(LED,HIGH);
    digitalWrite(ematch, HIGH);
    XBee.write("Ematch ignited ");
  }

  XBee.write("Temperature = ");
  XBee.write((int)temperature);
  XBee.write("Altitude = ");
  XBee.write((int)altitude);
  XBee.write("Ignite = ");
  XBee.write(ignite);
  /*
  Serial.print("Temperature = ");
  Serial.print((int)temperature);
  Serial.print("Altitude = ");
  Serial.print((int)altitude);
  Serial.print("Ignite = ");
  Serial.print(ignite);
  */
}

void IRAM_ATTR isr(){
  int buffer;  

  buffer = XBee.read();

  if(buffer==0x31){
    ignite |= 1; // have permission? set bit 1 high
    XBee.write("permission received ");
  }
  
  buffer = 0;
}