# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -Wpedantic -pthread

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Find all .c files in SRC_DIR and subdirectories
SRCS = $(shell find $(SRC_DIR) -name '*.c')
# Create a list of .o files based on the .c files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/main

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean