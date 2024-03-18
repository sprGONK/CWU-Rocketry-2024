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
#include "HardwareSerial.h"
#define RX 16
#define TX 17
const int MPU=0x68; 
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
bool permission
double pitch,roll;

HardwareSerial XBee(2); // UART2

void setup(){
    //accelerometer setup
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);
  Serial.begin(9600);

    //setup UART2 for xbee
  XBee.begin(9600,SERIAL_8N1,RX,TX);
  attachInterrupt(RX,isr,CHANGE);

  //Bluetooth setup
  //MS5607 setup
  //SD setup
}

void loop(){
    int[] accel = new int[7]; //possibly wrong syntax
    accel = readAccelerometer();
  
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