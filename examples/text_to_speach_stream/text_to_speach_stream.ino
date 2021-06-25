#include "flite_arduino.h"

// use FlitePrintStream for text output or FliteOutputStream for binary data
Flite flite(new FlitePrintStream(Serial));


void setup(){
    Serial.begin(115700);
}


void loop(){
    const char* hallo ="hallo";
    Serial.println(hallo);
    flite.say(hallo);
    
    delay(1000);
}