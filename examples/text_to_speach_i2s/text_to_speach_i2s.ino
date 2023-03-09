#include "flite_arduino.h" // install https://github.com/pschatzmann/arduino-flite
#include "AudioTools.h" // install https://github.com/pschatzmann/arduino-audio-tools
//#include "AudioLibs/AudioKit.h"

I2SStream out; // Replace with desired class e.g. AudioKitStream, AnalogAudioStream etc.
Flite flite(out);


void setup(){
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // start data sink
  auto cfg = out.defaultConfig();
  cfg.sample_rate = 8000;
  cfg.channels = 1;
  cfg.bits_per_sample = 16;
  out.begin(cfg);
}

// Arduino loop  
void loop() {  
  Serial.println("providing data...");
  flite.say("Hallo, my name is Alice,");
  delay(5000);
}