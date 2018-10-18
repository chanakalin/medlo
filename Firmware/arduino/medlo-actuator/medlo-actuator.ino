#include <Wire.h>

#define LED_R 2
#define LED_G 3
#define LED_B 4

void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  Wire.begin(8);                // join i2c bus with address #10
  Wire.onReceive(receiveEvent); // register event
}

void loop() {
  delay(100);
}

void receiveEvent(int howMany) {
  char c = 'a';
  c = Wire.read(); 
  

  if(c=='R'){
    digitalWrite(LED_R,HIGH);
  }
  if(c=='r'){
    digitalWrite(LED_R,LOW);
  }
  if(c=='G'){
    digitalWrite(LED_G,HIGH);
  }
  if(c=='g'){
    digitalWrite(LED_G,LOW);
  }
  if(c=='B'){
    digitalWrite(LED_B,HIGH);
  }
  if(c=='b'){
    digitalWrite(LED_B,LOW);
  }

}
