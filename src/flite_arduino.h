/**
 * @file flite_arduino.h
 * @author Phil Schatzmann
 * @brief Arduino API for Flite
 * @version 0.1
 * @date 2021-06-18
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "flite.h"
#include "flite_out_arduino.h"

// Declare available voices
extern "C" {
    cst_voice *register_cmu_us_kal(const char*voxdir=nullptr);
    cst_voice *register_cmu_us_kal16(const char* voxdir=nullptr);
    cst_voice *register_cmu_us_slt(const char *voxdir=nullptr);
    cst_voice *register_cmu_us_rms(const char *voxdir=nullptr);
    cst_voice *register_cmu_us_awb(const char *voxdir=nullptr);
    cst_voice *register_cmu_time_awb(const char *voxdir=nullptr);
};

// Arduino Output Driver
extern FliteOutputBase *arduino_output;

/**
 * @brief Arduino API for Flite Text to Speach Engine
 * 
 */
class Flite {
    public:
        /// Constructor - for text output to Serial
        Flite(){
            setOutput(new FlitePrintStream(Serial));
            flite_init();
        }

        /// Constructor - for output to callback
        Flite(flite_callback cb){
            setOutput(new FliteOutputCallback(cb));
            flite_init();
        }

        /// Constructor - for output to a stream
        Flite(Print &stream, bool as_text=false){
            if (as_text){
                setOutput(new FlitePrintStream(stream));
            } else {
                setOutput(new FliteOutputStream(stream));
            }
            flite_init();
        }

        /// Constructor - for output with a FliteOutputBase class
        Flite(FliteOutputBase *out){
            setOutput(out);
            flite_init();
        }

#ifdef ESP32
        /// Constructor - for output to I2S
        Flite(i2s_port_t i2s_num){
            setOutput(new FliteOutputI2S(i2s_num));
            flite_init();
        }
#endif

        /// Sets the voice 
        void setVoice(cst_voice *voice){
            this->voice = voice;
        }

        /// Defines the voice from a file or http url
        void setVoice(const char* url){
            this->voice = flite_voice_select(url);
        }

        /// Process text input
        void say(const char* text){
            setup();
            flite_text_to_speech(text, voice, outtype);
        }

        /// Process Flite explicit phones
        void sayPhones(const char* text){
            setup();
            flite_phones_to_speech(text, voice, outtype);
        }

        /// Process Speech Synthesis Markup Language (SSML) input
        void saySsml(const char* text){
            setup();
            flite_ssml_text_to_speech(text, voice, outtype);
        }

        /// convert text to WAV 
        cst_wave *textToWave(const char *text){
            setup();
            return flite_text_to_wave(text, voice);
        }

        /// provides access to output information
        FliteOutputBase* getOutput() {
            return arduino_output;
        }

        /// Delete voice and cst_audio_streaming_info
        void end() {
            delete_audio_streaming_info(asi);
            asi = nullptr;

            delete_voice(voice);
            voice = nullptr;
        }


    protected:
        cst_voice *voice = nullptr;
        const char *outtype = "play";
        cst_audio_streaming_info *asi=nullptr;

        void setup(){
            // setup default voice
            if (voice==nullptr){
                setVoice(register_cmu_us_kal());
                //setVoice(register_cmu_us_kal16(nullptr));
                //setVoice(register_cmu_us_slt(nullptr));
                //setVoice(register_cmu_us_rms(nullptr));
                //setVoice(register_cmu_us_awb(nullptr));
                //setVoice(register_cmu_time_awb(nullptr));
            }

            // setup streaming
            if (asi==nullptr){
                // setup streaming
                asi = new_audio_streaming_info();
                asi->asc = arduino_audio_stream_chunk;
                feat_set(voice->features, "streaming_info", audio_streaming_info_val(asi));
            }
        }

        void setOutput(FliteOutputBase *out){
            if (arduino_output!=nullptr)
                delete arduino_output;
            arduino_output = out;    
        }

};