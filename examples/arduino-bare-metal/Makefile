# Varaibles
PORT = /dev/cu.usbmodem11101
MCU         = atmega32u4
BAUD_RATE   = 57600
PROTOCOL    = avr109
CLOCK_S = 16000000UL 

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
# Compiler flags
CFLAGS = -Os -DF_CPU=$(CLOCK_S) -mmcu=$(MCU) 

# Default target: compile and generate hex file
default: $(BUILD_DIR)/$(BIN).hex

# Rule to create .hex file from .elf
$(BUILD_DIR)/$(BIN).hex: $(BUILD_DIR)/$(BIN).elf
	@mkdir -p $(BUILD_DIR)  # Ensure the build directory exists
	avr-objcopy -O ihex -R .eeprom $< $@

# Rule to create .elf file from object files
$(BUILD_DIR)/$(BIN).elf: $(OBJS)
	@mkdir -p $(BUILD_DIR)  # Ensure the build directory exists
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile .c files into .o object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)  # Ensure the obj directory exists
	$(CC) $(CFLAGS) -c $< -o $@

# Upload target (using avrdude)
# -vvvv verbose
upload: $(BUILD_DIR)/$(BIN).hex
	avrdude -F -V -c $(PROTOCOL) -p $(MCU) -P $(PORT) -b $(BAUD_RATE) -U flash:w:$< 

# Clean up object files and binaries
clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

# Print a list of object files
print-objects:
	@echo $(OBJS)
