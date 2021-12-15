/**
 * @file flite_out_arduino.h
 * @author Phil Schatzmann
 * @brief Arduino C++ Output Alternatives. 
 * @version 0.1
 * @date 2021-06-18
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "Arduino.h"
#include "flite.h"
#include "config.h"

#ifdef ESP32
#include "driver/i2s.h"
#include "freertos/queue.h"
#endif

// API callback
typedef void (*flite_callback)(size_t size, int16_t *values);

// define streaming callback used by Arduino
extern "C" int arduino_audio_stream_chunk(const cst_wave *w, int start, int size, int last, cst_audio_streaming_info_struct *user);


/**
 * @brief Base Output Class with common functionality
 * 
 */
class FliteOutputBase {
    public:
        virtual ~FliteOutputBase(){
        }

        virtual cst_audiodev * open(int sps, int channels, cst_audiofmt fmt) {
            LOG("FliteOutputBase::open: %d, %d, %d",  sps, channels, fmt);
            is_open = true;
            audiodev = cst_alloc(cst_audiodev, 1);
            audiodev->real_sps = audiodev->sps = sps;
            audiodev->real_channels = audiodev->channels = channels;
            audiodev->real_fmt = audiodev->fmt = fmt;
            audiodev->platform_data = (void *) this;

            return audiodev;
        };

        virtual int close() {
            is_open = false;
            return 0;
        }

        virtual int drain(){
            return 0;
        }

        virtual int flush(){
            return 0;
        }

        virtual int write(void *buff,int sample_count) = 0;

        int channels() {
            return audiodev!=nullptr ? audiodev->channels : 0;
        }

        int sampleRate() {
            return audiodev!=nullptr ? audiodev->real_sps : 0;
        }

        int bitsPerSample() {
            int result = 0;
            if (audiodev!=nullptr){
                switch(audiodev->real_fmt){
                    case 0: result = 16;
                        break;
                    case 1: result = 8;
                        break;
                    default:
                        Serial.println("Unsupported Audio Format");

                }
            }
            return result;
        }

        bool isOpen() {
            return is_open;
        }

        cst_audiodev* info() {
            return audiodev;
        }

    protected:
        bool is_open = false;
        cst_audiodev *audiodev = nullptr;
};

/**
 * @brief Output via Callback method
 * 
 */
class FliteOutputCallback : public  FliteOutputBase {
    public:
        FliteOutputCallback(flite_callback cb){
            callback = cb;
        }

        virtual int write(void *buffer,int sample_count) {
            LOG("FliteOutputCallback::write: %d",  sample_count);
            int size = sample_count;
            callback(size, (int16_t*) buffer);
            return 0;
        }

    protected:
        flite_callback callback;
};

#ifdef ESP32
/**
 * @brief Output to I2S for ESP32
 * 
 */
class FliteOutputI2S : public  FliteOutputBase {
    public:
        FliteOutputI2S( i2s_port_t i2s_num=I2S_NUM_0){
            this->i2s_num = i2s_num;
            setupDefaultConfig();
            setupDefaultPins();
        }

        FliteOutputI2S( i2s_port_t i2s_num, i2s_config_t cfg){
            this->i2s_num = i2s_num;
            this->i2s_config = cfg;
            setupDefaultPins();
        }

        FliteOutputI2S( i2s_port_t i2s_num, i2s_config_t cfg, i2s_pin_config_t pins ){
            this->i2s_num = i2s_num;
            this->i2s_config = cfg;
            this->pin_config = pins;
        }

        virtual cst_audiodev * open(int sample_rate, int channels, cst_audiofmt fmt) {
            // update sample sample_rate
            LOG("setting sample rate for I2S: %d", sample_rate);
            i2s_config.sample_rate = sample_rate;
            // install driver
            if (i2s_driver_install(i2s_num, &i2s_config, 0, NULL)!=ESP_OK){
                ESP_LOGE(TAG,"Failed to install i2s");
            }
            if (i2s_config.mode & I2S_MODE_DAC_BUILT_IN) {
                //for internal DAC, this will enable both of the internal channels
                LOG("i2s_set_pin: %s","internal DAC");
                if (i2s_set_pin(i2s_num, NULL)!=ESP_OK){
                    ESP_LOGE(TAG,"Failed to set i2s pins");
                }
            } else {
                // define pins for external DAC
                LOG("i2s_set_pin: %s","external DAC");
                if (i2s_set_pin(i2s_num, &pin_config)!=ESP_OK){
                    ESP_LOGE(TAG,"Failed to set i2s pins");
                }
            }
            return FliteOutputBase::open(sample_rate, channels, fmt);
        };

        virtual int close() {
            if (i2s_driver_uninstall(i2s_num) != ESP_OK){
                ESP_LOGE(TAG,"Failed to uninstall i2s");
            }

            return FliteOutputBase::close();
        }

        virtual int drain(){
            i2s_zero_dma_buffer(i2s_num);
            return 0;
        }

        virtual int write(void *buffer,int sample_count) {
            LOG("FliteOutputI2S::write: %d",  sample_count);
            size_t i2s_bytes_write;
            if (channels()==2){
                LOG("i2s_write: %s","simple call");
                if (i2s_write(i2s_num, buffer, sample_count*sizeof(int16_t), &i2s_bytes_write, portMAX_DELAY)!=ESP_OK){
                    ESP_LOGE(TAG,"i2s_write failed!");
                }
            } else {
                LOG("i2s_write: %s","generate data for 2 channels");
                // copy from 1 to 2 channels
                int total = 0;
                int16_t *ptr = (int16_t *) buffer;
                for (int j=0;j<sample_count;j++){
                    int16_t data[2] = {ptr[j], ptr[j]};
                    if (i2s_write(i2s_num, data, sizeof(int16_t)*2, &i2s_bytes_write, portMAX_DELAY)==ESP_OK){
                        total += i2s_bytes_write;
                    } else {
                        ESP_LOGE(TAG,"i2s_write failed!");
                    }
                }
                LOG("i2s_write - bytes written: %d",total);
            }
            return 0;
        }

    protected:
        i2s_port_t i2s_num;  
        i2s_config_t i2s_config;
        i2s_pin_config_t pin_config;
        const char *TAG = "FliteOutputI2S";

        void setupDefaultConfig() {
            const i2s_config_t i2s_config_default = {
                .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX),
                .sample_rate = this->sampleRate(),
                .bits_per_sample = (i2s_bits_per_sample_t)16,
                .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
                .communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
                .intr_alloc_flags = 0, // default interrupt priority
                .dma_buf_count = 8,
                .dma_buf_len = 64,
                .use_apll = false
            };
            this->i2s_config = i2s_config_default;
        }

        void setupDefaultPins() {
            static const i2s_pin_config_t pin_config_default = {
                .bck_io_num = 26,
                .ws_io_num = 25,
                .data_out_num = 22,
                .data_in_num = I2S_PIN_NO_CHANGE
            };
            this->pin_config = pin_config_default;
        }

};
#endif

/**
 * @brief Output using Arduino Print class
 * 
 */
class FliteOutputStream : public  FliteOutputBase {
    public:
        FliteOutputStream(Print &out){
            this->out_ptr = &out;
        }

        virtual int drain(){
            // while(out_ptr->available()>0){
            //     out_ptr->read();
            // }
            return 0;
        }

        virtual int flush(){
            //out_ptr->flush();
            return 0;
        };

        virtual int write(void *buffer, int sample_count) {
            LOG("FliteOutput::write: %d",  sample_count);
            out_ptr->write((const uint8_t *) buffer, sample_count*sizeof(int16_t));
            return 0;
        }

    protected:
        Print *out_ptr;       
};

/**
 * @brief Write readable string to Arduino Print class
 * 
 */
class FlitePrintStream : public  FliteOutputStream {
    public:
        FlitePrintStream( Print &out) : FliteOutputStream(out) {
        }

        virtual int write(void *buffer,int sample_count) {
            LOG("FlitePrintStream::write: %d",  sample_count);
            // copy from 1 to 2 channels
            int16_t *ptr = (int16_t *) buffer;
            for (int j=0;j<sample_count;j++){
                out_ptr->println(ptr[j]);
            }
            return 0;
        }

};

