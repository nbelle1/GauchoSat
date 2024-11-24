# Set the binary name (output)
BIN = led

# Source and object files
SRC_DIR = src
OBJ_DIR = obj
BUILD_DIR = build

SRC = $(wildcard $(SRC_DIR)/*.c)     # All .c files in src/
OBJS = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)  # Corresponding object files in obj/

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy

# Compiler flags
CFLAGS = -Os -DF_CPU=16000000UL -mmcu=atmega32u4

# Define the port for uploading (adjust for your setup)
PORT = /dev/tty.usbmodem11401

# Default target: compile and generate hex file
default: $(BUILD_DIR)/$(BIN).hex

# Rule to create .hex file from .elf
$(BUILD_DIR)/$(BIN).hex: $(BUILD_DIR)/$(BIN).elf
	@mkdir -p $(BUILD_DIR)  # Ensure the build directory exists
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Rule to create .elf file from object files
$(BUILD_DIR)/$(BIN).elf: $(OBJS)
	@mkdir -p $(BUILD_DIR)  # Ensure the build directory exists
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile .c files into .o object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)  # Ensure the obj directory exists
	$(CC) $(CFLAGS) -c $< -o $@

# Upload target (using avrdude)
install: $(BUILD_DIR)/$(BIN).hex
	avrdude -v -F -V -c arduino -p ATMEGA32U4 -P $(PORT) -b 115200 -U flash:w:$<

# Clean up object files and binaries
clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

# Print a list of object files
print-objects:
	@echo $(OBJS)
