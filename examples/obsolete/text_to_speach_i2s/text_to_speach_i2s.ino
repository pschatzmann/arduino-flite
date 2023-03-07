#include "flite_arduino.h"

Flite flite(new FliteOutputI2S());


void setup(){
    Serial.begin(115700);
}


void loop(){
    const char* hallo ="hallo";
    Serial.println(hallo);
    flite.say(hallo);
    delay(1000);
}