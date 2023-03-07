/**
 * @file text_to_speach_i2s_analog.ino
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

const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
    .sample_rate = 0, // dynamically update
    .bits_per_sample = (i2s_bits_per_sample_t)16,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
};

Flite flite(new FliteOutputI2S(I2S_NUM_0, i2s_config));

void setup(){
    Serial.begin(115700);
}


void loop(){
    const char* hallo ="hallo";
    Serial.println(hallo);
    flite.say(hallo);
    delay(1000);
}