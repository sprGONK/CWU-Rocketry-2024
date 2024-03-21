// Code taken from https://learn.adafruit.com/adafruit-micro-sd-breakout-board-card-tutorial/arduino-library
#include "SD.h"
#define SS 5
#define sdFileName "/flightData_3-20-24.json"
File myFile;
// Variables for timer after launch started
unsigned long startTime = 0;
unsigned long timeInterval = 1000; // Will record data every 
 
void setup()
{
  delay(5000); // time for me to open the serial monitor
  Serial.begin(9600);
  /*
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(SS, OUTPUT);
 
  if (!SD.begin(SS)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("/test.txt", FILE_WRITE);
 
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  } */

  //SD setup
  Serial.print("Initializing SD card...");
  
  // Setup 
  pinMode(SS, OUTPUT);
 
  // Verify connection was made
  if (!SD.begin(SS)) {
    Serial.println("initialization failed!");
  } else {
    Serial.println("initialization done.");

    myFile = SD.open(sdFileName, FILE_WRITE);

    // Close the file after writing initial data
    myFile.close();
    Serial.print(sdFileName);
    Serial.println(" Created");

	startTime = millis(); // Start the timer
  }
}
 
void loop()
{
	// Open the JSON file in append mode
	myFile = SD.open(sdFileName, FILE_APPEND);

	if (!myFile) {
		Serial.print("Error opening ");
    Serial.println(sdFileName);
	} else {
		// Calculate the current time based on the elapsed time since the start
		unsigned long currentTime = ((millis() - startTime) / 1000);

		// Add JSON data for the current time slot
		myFile.println("{");
		myFile.print("    \"");
		myFile.print(String(currentTime));
		myFile.println("\": {");
		myFile.println("        \"altitude\": \"100 meters\",");
		myFile.println("        \"pressure\": \"1013.25 hPa\",");
		myFile.println("        \"temperature\": \"25°C\"");
		myFile.print("    }");
		myFile.println();

		// Close the file after appending data
		myFile.close();
		Serial.print("Data appended to ");
    Serial.println(sdFileName);

		delay(timeInterval);
	}
}