int LED = 2;

void setup(){
  pinMode(LED,OUTPUT);
}

void loop(){
  digitalWrite(LED,HIGH);
  Delay(1000);
  digitalWrite(LED,LOW);
  Delay(1000);
}