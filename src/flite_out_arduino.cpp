/**
 * @file flite_out_arduino.cpp
 * @author Phil Schatzmann
 * @brief Arduino Callback Method to output generated voice data
 * @version 0.1
 * @date 2021-06-18
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "flite_out_arduino.h"

extern "C" {
#include "audio/native_audio.h"

FliteOutputBase *arduino_output = nullptr;

/**
 * @brief flite streaming callback method 
 */
int arduino_audio_stream_chunk(const cst_wave *w, int start, int size, int last, cst_audio_streaming_info_struct *user) {
    LOG("arduino_audio_stream_chunk - start: %d, size: %d", start, size);

    if (arduino_output!=nullptr){
        if (!arduino_output->isOpen()){
            arduino_output->open(w->sample_rate, w->num_channels, CST_AUDIO_LINEAR16);
        }

        arduino_output->write(w->samples+start, size);

        if (last){
            LOG("arduino_audio_stream_chunk - %s","done");
        }
    }
    return 0;
}


}
