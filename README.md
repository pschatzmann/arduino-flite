# Flite: a small speech synthesis engine library for Arduino

Flite (festival-lite) is a small, fast run-time open source text to speech synthesis engine developed at Carnegie Mellon University (CMU) and primarily designed for small embedded machines and/or large servers. Flite is designed as an alternative text to speech synthesis engine to Festival for voices built using the FestVox suite of voice building tools. 

I created this project with the intention to provide Flite as Arduino Library which supports different output alternatives:

- [Output to I2S (for the ESP32)](examples/text_to_speach_i2s/text_to_speach_i2s.ino)
- [Output to a Arduino Stream](examples/text_to_speach_stream/text_to_speach_stream.ino)
- [Output with a callback method](examples/text_to_speach_callback/text_to_speach_callback.ino)

## Installation

You can download the library as zip and call include Library -> zip library. Or you can git clone this project into the Arduino libraries folder e.g. with
```
cd  ~/Documents/Arduino/libraries
git clone pschatzmann/arduino-flite.git
```

## Documentation

Here is the generated [Class documentation](https://pschatzmann.github.io/flite/doc/html/class_flite.html) for the Arduino API. 
Further information on Flite can be found [in the FLITE.md](FLITE.md)

## Output Format

The generated output format is usually as follows

|  Parameter        | Value |
|-------------------|-------|
|  bits_per_sample  |   16  |
|  channels         |    1  |
|  sample_rate      | 8000  |


## Memory Requirements

The memory requirements depend very much on the selected voice. Currently the minimal voice is cmu_us_kal:

| voice         | Progmem | Dynamic Memory |
|---------------|---------|----------------|
| cmu_us_kal    | 2.34M   | 16632          |  


The requirements have been determined by compiling the project for an ESP32. Please note that all voices are exeeding the possibilities of most microcontrollers, so this project project is of limited value and I try to find some better alternatives. 

## Licence

see [COPYING](COPYING)

