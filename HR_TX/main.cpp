#include <Arduino.h>
#include "IRsnd.h"
#include "Encode.h"

unsigned long tx_id = 7543123; //Set your unique 7 digit Transponder ID here

int khz = 426;
int powerLED = 10;
int ledState = LOW;
unsigned long previousMillis = 0; 
unsigned long currentMillis = 0;

IRsend irsend;

void setup() {
  Serial.begin(115200);
  pinMode(powerLED, OUTPUT);
  makeOutputCode(tx_id); // convert TX_ID to code just once
}

void loop() {
  irsend.sendRaw(outputcode, codeLen, khz);
  delay(7 + random(2, 5));
  currentMillis = millis();
  if (currentMillis - previousMillis >= 200) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } 
    else {
      ledState = LOW;
    }
    digitalWrite(powerLED, ledState);
  }
  delayMicroseconds(500);
}
