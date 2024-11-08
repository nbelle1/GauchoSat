#include <Arduino.h>

// Define the LED pin. LED_BUILTIN is predefined in Arduino environments.
// It usually refers to pin 13 on most Arduino boards.
const int ledPin = LED_BUILTIN;

// Define the delay time in milliseconds for rapid blinking
const unsigned long blinkDelay = 100; // 100 ms

void setup() {
  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Turn the LED on
  digitalWrite(ledPin, HIGH);
  
  // Wait for the specified delay time
  delay(blinkDelay);
  
  // Turn the LED off
  digitalWrite(ledPin, LOW);
  
  // Wait for the specified delay time
  delay(blinkDelay);
}
