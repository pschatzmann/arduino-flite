#include "flite_arduino.h"

void callback(size_t len, int16_t *data){
    for (size_t j=0;j<len;j++){
        Serial.println(data[j]);
    }
}

Flite flite(callback);

void setup(){
    Serial.begin(115700);
}


void loop(){
    const char* hallo ="hallo";
    Serial.println(hallo);
    flite.say(hallo);
    
    delay(1000);
}