/*
Notes:

TODO:
add bluetooth
add active/sleep modes controlled by bluetooth (not sure how I want to accomplish)
  looks like this isn't a thing
*/
// We'll use HardwareSerial to communicate with the XBee:
#include "HardwareSerial.h"
#include "Adafruit_MPL3115A2.h"
#include "BluetoothSerial.h"
//UART Pins
#define RX 16
#define TX 17
#define LED 2
#define ematch 15

//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "ESP32-BT-Slave";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBTS;
Adafruit_MPL3115A2 baro;

/* ignite global permission, has 4 checks
  Primer(ascent), Range, and Permission
  Primer: checking if rocket is above 1000ft
  Range: checks if we're in ejection range of 400-550ft only after Primer has happened
  Permission: checks if we've gotten ground permission
  */
int ignite;
float baseAltitude;

void IRAM_ATTR isr();
float getCurrentAltitude();

void setup()
{
  delay(5000);
  //setup GPIO for ematch
  pinMode(LED,OUTPUT);
  pinMode(ematch,OUTPUT);

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

  ignite = 1; //hard set permission

  //setup Bluetooth
  SerialBTS.begin(device_name);
  Serial.println("Bluetooth is on"); //Bluetooth device name
  #ifdef USE_PIN
    SerialBTS.setPin(pin);
    Serial.println("Using PIN");
  #endif

  /* while(!SerialBTS.available()){
    //sit here until something is received, should save power ? ?
  } */
}

void loop(){
  Serial.println("I'm working");
  if(SerialBTS.available()){
    if(SerialBTS.read()==0x31){
      ignite |= 1;
    }
  }

  float pressure = baro.getPressure();
  float altitude = getCurrentAltitude();
  float temperature = baro.getTemperature();
  SerialBTS.write(altitude);

  if(altitude > 1000){
    ignite |= 4; //above 1000ft? set bit 3 high
  }
  if(ignite >= 4){ // are bits 3 and or 1 high? proceed
    if((altitude < 450) && (altitude > 400)){
      ignite |= 2; // in ejection altitude range? set bit 2 high
    }else{
      ignite = 4; //TODO: properly write setting bit 2 low
    }
  }
  
  if(ignite == 7){ //are all 3 bits high? ignite the ematch
    digitalWrite(LED,HIGH);
    digitalWrite(ematch, HIGH);
    //SerialBTS.write("Ematch ignited ");
  }
}

float getCurrentAltitude(){
  return baro.getAltitude()*3.28 - baseAltitude;
}