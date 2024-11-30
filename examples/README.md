# Necessary Libraries and Setup for Arduino Micro (Atmega32u4)

## 1. Install Homebrew (if not already installed)
   Visit https://brew.sh for installation instructions.

## 2. Install the Required Libraries
   Run the following commands in your terminal:

   Install AVR-GCC (AVR Toolchain for compiling code for Atmega32u4):
   brew install avr-gcc

   Install AVRDUDE (used to upload code to the Arduino Micro):
   brew install avr-dude

## 3. To run code on Arduino Micro (Atmega32u4):
   - Press the "reset" button on the Arduino Micro to prepare it for uploading.
   - In your project directory, run the following command to upload the compiled code:

     make upload

   This will compile the code and upload it to the Arduino Micro via AVRDUDE.
