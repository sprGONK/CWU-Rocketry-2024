#define LED 2
#define MOSFET 15

int stateLED;

void setup(){
    pinMode(15,OUTPUT);
    pinMode(2,OUTPUT);
    Serial.begin(9600);
    while(!Serial);
}
void loop(){
  if (Serial.available() > 0) {
    byte incomingByte = Serial.read();
    switch (incomingByte){
    case '0':
      Serial.println("toggle");
        digitalWrite(15,!digitalRead(15));
        digitalWrite(2,!digitalRead(2));
      break;
    } //endswitch
  }//enif
}