#define LED 2 //GPIO 2 is onboard LED

void setup(){
  pinMode(LED,OUTPUT);
}

void loop(){
  digitalWrite(LED,HIGH);
  delay(100); // in ms
  digitalWrite(LED,LOW);
  delay(100);
}