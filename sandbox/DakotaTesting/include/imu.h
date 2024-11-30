#ifndef IMU_H
#define IMU_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// Global declarations
extern Adafruit_BNO055 bno; // Declare the sensor object for external use
extern uint8_t ledStat;

// Function declarations
void IMUinit();
void IMUdemo();
void toggleLED();

#endif
