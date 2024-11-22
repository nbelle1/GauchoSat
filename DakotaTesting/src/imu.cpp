#include "imu.h"

// Define global variables
Adafruit_BNO055 bno = Adafruit_BNO055(55);
uint8_t ledStat = 0x0;

void IMUinit() {
    Serial.begin(9600);
    Serial.println("Orientation Sensor Test"); 
    Serial.println("");

    // Initialize the sensor
    if (!bno.begin()) {
        Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while (1);
    }

    delay(1000);
    bno.setExtCrystalUse(true);

    // Set LED pin as output
    pinMode(LED_BUILTIN, OUTPUT);
}

void IMUdemo() {
    // Get a new sensor event
    sensors_event_t event;
    bno.getEvent(&event);

    // Display the floating-point data
    Serial.print("X: ");
    Serial.print(event.orientation.x, 4);
    Serial.print("\tY: ");
    Serial.print(event.orientation.y, 4);
    Serial.print("\tZ: ");
    Serial.print(event.orientation.z, 4);
    Serial.println("");
}

void toggleLED() {
    ledStat ^= 0x1; // Toggle LED state
    digitalWrite(LED_BUILTIN, ledStat);
}
