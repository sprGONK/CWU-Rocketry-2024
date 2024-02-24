// We'll use HardwareSerial to communicate with the XBee:
#include <HardwareSerial.h>
#define RX 16
#define TX 17
#define LED 2
#define command 15

HardwareSerial XBee(2); // UART2

void IRAM_ATTR eject();

void setup()
{
  // Set up both ports at 9600 baud. This value is most important
  // for the XBee. Make sure the baud rate matches the config
  // setting of your XBee.
  XBee.begin(9600,SERIAL_8N1,RX,TX);
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
  pinMode(command,OUTPUT);
  attachInterrupt(RX,eject,CHANGE);
}

int buffer;

void loop()
{
  buffer = Serial.read();
  XBee.write("test");
  Serial.println(buffer, HEX);
  buffer = 0;
  delay(500);
}

void IRAM_ATTR eject(){
  buffer = XBee.read();
  if(buffer == 0x31){
    digitalWrite(LED,!digitalRead(LED));
    digitalWrite(command,!digitalRead(command));
  }
  buffer = 0;
}
