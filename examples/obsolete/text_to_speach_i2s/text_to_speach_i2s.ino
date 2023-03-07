/**
 * @file text_to_speach_i2s.ino
 * @author Phil Schatzmann (you@domain.com)
 * @brief Obsolete: Works only on regular ESP32 and after setting 
 * #define ESP32_I2S_ACTIVE true
 * in config.h
 * 
 * @version 0.1
 * @date 2023-03-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
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