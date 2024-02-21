/*****************************************************************
XBee_Serial_Passthrough.ino

Set up a software serial port to pass data between an XBee Shield
and the serial monitor.

Hardware Hookup:
  The XBee Shield makes all of the connections you'll need
  between Arduino and XBee. If you have the shield make
  sure the SWITCH IS IN THE "DLINE" POSITION. That will connect
  the XBee's DOUT and DIN pins to Arduino pins 2 and 3.

*****************************************************************/
// We'll use SoftwareSerial to communicate with the XBee:
#include <HardwareSerial.h>
#define RX 16
#define TX 17
#define LED 2
//For Atmega328P's
// XBee's DOUT (TX) is connected to pin 2 (Arduino's Software RX)
// XBee's DIN (RX) is connected to pin 3 (Arduino's Software TX)
HardwareSerial XBee(2); // RX, TX

//For Atmega2560, ATmega32U4, etc.
// XBee's DOUT (TX) is connected to pin 10 (Arduino's Software RX)
// XBee's DIN (RX) is connected to pin 11 (Arduino's Software TX)
//SoftwareSerial XBee(10, 11); // RX, TX

void setup()
{
  // Set up both ports at 9600 baud. This value is most important
  // for the XBee. Make sure the baud rate matches the config
  // setting of your XBee.
  XBee.begin(9600,SERIAL_8N1,RX,TX);
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
}

int buffer;

void loop()
{
  if (Serial.available())
  { // If data comes in from serial monitor, send it out to XBee
    buffer = Serial.read();
    XBee.write(buffer);
    digitalWrite(LED,HIGH);
    Serial.println(buffer, HEX);
    buffer = 0;
  }
  if (XBee.available())
  { // If data comes in from XBee, send it out to serial monitor
    buffer = XBee.read();
    Serial.write(buffer);
    digitalWrite(LED,LOW);
    Serial.println(buffer, HEX);
    buffer = 0;

  }
}
