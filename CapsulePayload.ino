/*
Notes:
not sure where I want to add SD card, probably in here
TODO:
add bluetooth
add active/sleep modes controlled by XBee
add SD card
add MS5607
*/

#include <Wire.h>
#include <math.h> 
#include <string.h>
#include "HardwareSerial.h"
#include "BluetoothSerial.h"
//UART2 Pins
#define RX 16
#define TX 17
//SPI Pins
#define SS 5
#define SCK 18
#define MISO 19
#define MOSI 23

#define USE_NAME // Comment this to use MAC address instead of a slaveName
const char *pin = "1234"; // Change this to reflect the pin expected by the real slave BT device

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

const int MPU=0x68; 
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
bool permission
double pitch,roll;
String myName = "ESP32-BT-Master";

HardwareSerial XBee(2); // UART2
BluetoothSerial SerialBTM;

#ifdef USE_NAME
  String slaveName = "ESP32-BT-Slave"; // Change this to reflect the real name of your slave BT device
#else
  String MACadd = "AA:BB:CC:11:22:33"; // This only for printing
  uint8_t address[6]  = {0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33}; // Change this to reflect real MAC address of your slave BT device
#endif

void setup(){
    //accelerometer setup
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);
  Serial.begin(115200);

    //setup UART2 for xbee
  XBee.begin(115200,SERIAL_8N1,RX,TX);
  attachInterrupt(RX,isr,CHANGE);

  //Bluetooth setup
  bool connected;
  SerialBTM.begin(myName, true);
  #ifndef USE_NAME
    SerialBTM.setPin(pin);
    Serial.println("Using PIN");
  #endif

  // connect(address) is fast (up to 10 secs max), connect(slaveName) is slow (up to 30 secs max) as it needs
  // to resolve slaveName to address first, but it allows to connect to different devices with the same name.
  // Set CoreDebugLevel to Info to view devices Bluetooth address and device names
  #ifdef USE_NAME
    connected = SerialBTM.connect(slaveName);
    Serial.printf("Connecting to slave BT device named \"%s\"\n", slaveName.c_str());
  #else
    connected = SerialBTM.connect(address);
    Serial.print("Connecting to slave BT device with MAC "); Serial.println(MACadd);
  #endif

  if(connected) {
    Serial.println("Connected Successfully!");
  } else {
    while(!SerialBTM.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
    }
  }
  // Disconnect() may take up to 10 secs max
  if (SerialBTM.disconnect()) {
    Serial.println("Disconnected Successfully!");
  }
  // This would reconnect to the slaveName(will use address, if resolved) or address used with connect(slaveName/address).
  SerialBTM.connect();
  if(connected) {
    Serial.println("Reconnected Successfully!");
  } else {
    while(!SerialBTM.connected(10000)) {
      Serial.println("Failed to reconnect. Make sure remote device is available and in range, then restart app.");
    }
  }
  //MS5607 setup
  //SD setup
}

void loop(){
    int[] accel = new int[7]; //possibly wrong syntax
    accel = readAccelerometer();

    if(permission){
      //send permission to Upper

    }
  
}

int[] readAccelerometer(){
    Wire.beginTransmission(MPU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14,true);  
  
  int AcXoff,AcYoff,AcZoff,GyXoff,GyYoff,GyZoff;
  int temp,toff;
  double t,tx,tf;
  
//Acceleration data correction
  AcXoff = -950;
  AcYoff = -300;
  AcZoff = 0;

//Temperature correction
  toff = -1600;

//Gyro correction
  GyXoff = 480;
  GyYoff = 170;
  GyZoff = 210;

//read accel data  
  AcX=(Wire.read()<<8|Wire.read()) + AcXoff;    
  AcY=(Wire.read()<<8|Wire.read()) + AcYoff;  
  AcZ=(Wire.read()<<8|Wire.read()) + AcYoff;  

//read temperature data
  temp=(Wire.read()<<8|Wire.read()) + toff;  
  tx=temp;
  t = tx/340 + 36.53;
  tf = (t * 9/5) + 32; //temperature fahrenheit

//read gyro data
  GyX=(Wire.read()<<8|Wire.read()) + GyXoff;  
  GyY=(Wire.read()<<8|Wire.read()) + GyYoff;  
  GyZ=(Wire.read()<<8|Wire.read()) + GyZoff;  
    
//get pitch/roll
  getAngle(AcX,AcY,AcZ);
  return [AcX, AcY, AcZ, tf, GyX, GyY, GyZ]; //probably wrong syntax
}

//convert the accel data to pitch/roll
void getAngle(int Vx,int Vy,int Vz) {
    double x = Vx;
    double y = Vy;
    double z = Vz;

    pitch = atan(x/sqrt((y*y) + (z*z)));
    roll = atan(y/sqrt((x*x) + (z*z)));
    //convert radians into degrees
    pitch = pitch * (180.0/3.14);
    roll = roll * (180.0/3.14) ;
}

void IRAM_ATTR ISR(){
  int buffer;  

  buffer = XBee.read();

  if(buffer==0x31){
    permission = true; // have permission? set true
    XBee.write("permission received ");
  }
  
  buffer = 0;
}