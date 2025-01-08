# Set the compiler
CC = gcc

# Set the compiler flags (e.g., -Wall for all warnings)
CFLAGS = -Iinclude -Ilib/qpn_avr -Wall -Wextra -g

# Set the directories
LIB_DIR = lib
SRC_DIR = src
OBJ_DIR = obj
QPN_DIR = lib/qpn_avr

# Set the output binary name
OUTPUT = simulation

# Find all .c files in the src, lib, and qpn_avr directories
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
LIB_FILES = $(wildcard $(LIB_DIR)/*.c)
QPN_FILES = $(wildcard $(QPN_DIR)/*.c)

# Object files placed directly in OBJ_DIR (flattening the file structure)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES)) \
            $(patsubst $(LIB_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_FILES)) \
            $(patsubst $(QPN_DIR)/%.c, $(OBJ_DIR)/%.o, $(QPN_FILES))

# Default target
all: $(OUTPUT)

# Link object files into the final executable
$(OUTPUT): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(OUTPUT)

# Compile source files into object files in OBJ_DIR
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(QPN_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the output binary (but keep the obj folder)
clean:
	find $(OBJ_DIR) -type f -name '*.o' -delete
	rm -f $(OUTPUT)
