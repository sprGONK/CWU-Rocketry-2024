/*
Notes:
not sure where I want to add SD card, probably in here
TODO:
add bluetooth
add active/sleep modes controlled by XBee
add SD card (done) Maybe add accelerometer data to be saved to this?
add MS5607
*/

#include <Wire.h>
#include <math.h> 
#include <string.h>
#include "HardwareSerial.h"
#include "BluetoothSerial.h"
#include "Adafruit_MPU6050.h"
#include "SD.h"
//UART2 Pins
#define RX 16
#define TX 17
//SPI Pins
#define SS 5
#define SCK 18
#define MISO 19
#define MOSI 23
// SD Card Reader
#define sdFileName "/payloadFlightData_3-21-24.csv"
#define csvDataStructure "Time, Altitude, Temperature, Pressure" // Order is important, otherwise there will confusion of data
unsigned long sdDataStartTime = 0;
// LED Light
#define LED 2 //GPIO 2 is onboard LED

#define USE_NAME // Comment this to use MAC address instead of a slaveName
const char *pin = "1234"; // Change this to reflect the pin expected by the real slave BT device

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

File myFile;
bool permission
String myName = "ESP32-BT-Master";
bool connected; // Variable for whether or not the bluetooth is connected

HardwareSerial XBee(2); // UART2
BluetoothSerial SerialBTM; // Bluetooth channel
Adafruit_MPU6050 mpu;

#ifdef USE_NAME
  String slaveName = "ESP32-BT-Slave"; // Change this to reflect the real name of your slave BT device
#else
  String MACadd = "AA:BB:CC:11:22:33"; // This only for printing
  uint8_t address[6]  = {0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33}; // Change this to reflect real MAC address of your slave BT device
#endif

void IRAM_ATTR ISR();

void setup(){
  // LED Setup
  pinMode(LED,OUTPUT);

  //accelerometer setup
  Serial.begin(115200);
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);


  //setup UART2 for xbee
  XBee.begin(115200,SERIAL_8N1,RX,TX);
  attachInterrupt(RX,isr,CHANGE);

  //SD setup
  Serial.print("Initializing SD card...");
  pinMode(SS, OUTPUT);
 
  // Verify connection was made
  if (!SD.begin(SS)) {
    Serial.println("initialization failed!");
  } else {
    Serial.println("initialization done.");

    // Open File
    myFile = SD.open(sdFileName, FILE_WRITE);

    // Write structure of file into file first
    myFile.println(csvDataStructure);

    // Close the file after writing initial data
    myFile.close();
    Serial.print(sdFileName);
    Serial.println(" Created");
  }

  //Bluetooth setup
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
    digitalWrite(LED,HIGH);
  } else {
    while(!SerialBTM.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
      digitalWrite(LED,LOW);
    }
  }
  // Disconnect() may take up to 10 secs max
  if (SerialBTM.disconnect()) {
    Serial.println("Disconnected Successfully!");
    digitalWrite(LED,LOW);
  }
  // This would reconnect to the slaveName(will use address, if resolved) or address used with connect(slaveName/address).
  SerialBTM.connect();
  if(connected) {
    Serial.println("Reconnected Successfully!");
    digitalWrite(LED,HIGH);
  } else {
    while(!SerialBTM.connected(10000)) {
      Serial.println("Failed to reconnect. Make sure remote device is available and in range, then restart app.");
      digitalWrite(LED,LOW);
    }
  }
  //MS5607 setup

}

void loop(){
  if(permission){
    //send permission to Upper
    SerialBTM.write(0x31);
  }
  
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  writeDataToSD()
}

boolean writeDataToSD(){
  // Open the JSON file in append mode
	myFile = SD.open(sdFileName, FILE_APPEND);

	if (!myFile) {
		Serial.print("Error opening ");
    Serial.println(sdFileName);
    return false;
	} else {
		// Calculate the current time based on the elapsed time since the start
		unsigned long currentTime = millis() - startTime;

		// Add JSON data for the current time slot
		myFile.print(String(currentTime));
    myFile.print(",");
		myFile.print(String(""));             // Replace with variable name for altitude
    myFile.print(",");
		myFile.print(String(""));             // Replace with variable name for temperature
    myFile.print(",");
		myFile.print(String(""));             // Replace with variable name for pressure
    myFile.print(",");
    myFile.print(String(connected));
		myFile.println();

		// Close the file after appending data
		myFile.close();
		Serial.print("Data appended to ");
    Serial.println(sdFileName);
    return true;
  }
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