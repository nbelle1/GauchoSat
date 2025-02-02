# Project Configuration
PORT        = /dev/cu.usbmodem11101
MCU         = atmega32u4
F_CPU       = 16000000UL
BAUD_RATE   = 115200
PROTOCOL    = avr109

# Project-specific details
TARGET      = main
SRC_DIR     = src
OBJ_DIR     = obj
BUILD_DIR   = build

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Compiler and tools
CC = avr-gcc
CFLAGS = -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LD_FLAGS = 

# Default target: compile and generate hex file
default: $(BUILD_DIR)/$(TARGET).hex

# Rule to create .hex file from .elf
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	@mkdir -p $(BUILD_DIR)
	avr-objcopy -O ihex -R .eeprom $< $@

# Rule to create .elf file from object files
$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LD_FLAGS) -o $@ $^

# Rule to compile .c files into .o object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Upload target (using avrdude)
upload: $(BUILD_DIR)/$(TARGET).hex
	avrdude -F -V -c $(PROTOCOL) -p $(MCU) -P $(PORT) -b $(BAUD_RATE) -U flash:w:$(BUILD_DIR)/$(TARGET).hex 

# Clean up object files and binaries
clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

# Print a list of object files
print-objects:
	@echo $(OBJS)
