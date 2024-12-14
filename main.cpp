#include <Arduino.h>
#include "IRsnd.h"
#include "Encode.h"

unsigned long tx_id = 6262626; //Set 7 digit Transponder ID here

//boolean Testing = true; // set to true for testing
int khz = 426;
int powerLED = 10;
int ledState = LOW;
unsigned long previousMillis = 0; 
unsigned long currentMillis = 0;

IRsend irsend;

void setup() {
  Serial.begin(115200);
  pinMode(powerLED, OUTPUT);
/*   if (Testing == true){
    khz = 38; //to match NEC spec
  } 
  else{
    khz = 426; //to match i-Laps spec
    makeOutputCode(tx_id); // convert TX_ID to code just once
  } */
 // khz = 426; //to match i-Laps spec
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
  // if (Testing == true){
  //   makeOutputCode(tx_id); // convert TX_ID to code each loop for testing purposes
  //   // Dont forget to enable "#define debug" in Encode.h to send debug info to serial monitor
  //   delay(1000); // 1 sec delay between pulses for testing purposes
  // } 
  // else{
  //   delayMicroseconds(500);
  // }
  delayMicroseconds(500);
}
