#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// Define the I2C address
#define BNO055_ADDRESS BNO055_ADDRESS_A // For 0x28
// #define BNO055_ADDRESS BNO055_ADDRESS_B // For 0x29

// Create an instance of the BNO055 sensor with the specified address
Adafruit_BNO055 bno = Adafruit_BNO055(55, BNO055_ADDRESS);

// Define the LED pin for status indication (Arduino Micro LED is on pin 17)
const int ledPin = 17; // Arduino Micro built-in LED pin

void setup(void) {
  // Initialize Serial communication at 9600 baud rate
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for Serial port to connect. Needed for native USB
  }

  // Initialize the built-in LED pin as an output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Ensure LED is off initially

  // Initialize the BNO055 sensor
  if (!bno.begin()) {
    // If initialization fails, print error and blink LED rapidly
    Serial.println("No BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }
  }

  // Set the sensor to operate in NDOF (Nine Degrees of Freedom) mode
  bno.setExtCrystalUse(true);

  Serial.println("BNO055 successfully initialized!");
  digitalWrite(ledPin, HIGH); // Turn on LED to indicate successful initialization
  delay(1000);
  digitalWrite(ledPin, LOW); // Turn off LED
}

void loop(void) {
  // Retrieve the sensor event
  sensors_event_t event;
  bno.getEvent(&event);

  // Retrieve the orientation data (Euler angles)
  imu::Quaternion quat = bno.getQuat();
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  // Print the orientation data to the Serial Monitor
  Serial.print("Orientation: ");
  Serial.print((int)euler.x());
  Serial.print("°, ");
  Serial.print((int)euler.y());
  Serial.print("°, ");
  Serial.print((int)euler.z());
  Serial.println("°");

  // Print quaternion data
  Serial.print("Quaternion: ");
  Serial.print(quat.x(), 4);
  Serial.print(", ");
  Serial.print(quat.y(), 4);
  Serial.print(", ");
  Serial.print(quat.z(), 4);
  Serial.print(", ");
  Serial.println(quat.w(), 4);

  // Optional: Blink the LED to indicate data is being read
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
}
