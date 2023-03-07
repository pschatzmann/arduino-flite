/**
 * @file config.h
 * @brief Configuration settings for Arduino
 */

#pragma once

// Logging
//#define LOG(fmt,...)
#define LOG(fmt,...) Serial.printf(fmt, __VA_ARGS__); Serial.println();

// Deactivate Sockets
#define CST_NO_SOCKETS


// Activate/Deactivate built in I2S 
#define ESP32_I2S_ACTIVE false
