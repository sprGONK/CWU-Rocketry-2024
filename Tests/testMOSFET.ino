#define LED 2
#define MOSFET 15

int stateLED;

void setup(){
    pinMode(15,OUTPUT);
    pinMode(2,OUTPUT);
    Serial.begin(9600);
    while(!Serial);
    Serial.flush();
}
void loop(){
    digitalWrite(15,!digitalRead(15));
    digitalWrite(2,!digitalRead(2));
    delay(1000);
}