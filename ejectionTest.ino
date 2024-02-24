// We'll use HardwareSerial to communicate with the XBee:
#include <HardwareSerial.h>
#define RX 16
#define TX 17
#define LED 2
#define ematch 15

HardwareSerial XBee(2); // UART2
Adafruit_MPL3115A2 baro; // altimeter

void IRAM_ATTR eject(); // GPIO interrupt

void setup()
{
  XBee.begin(9600,SERIAL_8N1,RX,TX);
  pinMode(LED,OUTPUT);
  pinMode(ematch,OUTPUT);
  attachInterrupt(RX,eject,CHANGE);
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
  baro.setSeaPressure(1013.26);
}

float altitude;

void loop(){
  float pressure = baro.getPressure();
  altitude = baro.getAltitude();
  float temperature = baro.getTemperature();

  XBee.write("Temperature = ");
  Xbee.write((int)temperature);
  Xbee.write("Altitude = ");
  Xbee.write((int)altitude*3.28);
}

void IRAM_ATTR eject(){
  int dummy, permission buffer;
  buffer = XBee.read();
  if(buffer==0x31){
    permission |= 1;
  }
  while(altitude < 800/3.28){
    dummy += 1;
  }
  while(!permission || (altitude<550/3.28||altitude>400)){
    dummy += 1;
  }
  if(buffer == 0x31 && height == ){
    digitalWrite(LED,!digitalRead(LED));
    digitalWrite(ematch,!digitalRead(ematch));
  }
  buffer = 0;
}
