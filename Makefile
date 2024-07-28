# Compiler and flags
CC = gcc
CFLAGS_COMMON = -std=c18 -Wall -Wextra -Wpedantic -pthread
CFLAGS_DEV = -g -Werror -fsanitize=address
CFLAGS_RELEASE = -O2

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Find all .c files in SRC_DIR and subdirectories
SRCS = $(shell find $(SRC_DIR) -name '*.c')
# Create a list of .o files based on the .c files
OBJS_DEV = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/dev/%.o,$(SRCS))
OBJS_RELEASE = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/release/%.o,$(SRCS))

# Default target
all: dev release

# Development build
dev: CFLAGS=$(CFLAGS_COMMON) $(CFLAGS_DEV)
dev: $(BIN_DIR)/main_dev

# Release build
release: CFLAGS=$(CFLAGS_COMMON) $(CFLAGS_RELEASE)
release: $(BIN_DIR)/main_release

# Link object files to create the executable for development
$(BIN_DIR)/main_dev: $(OBJS_DEV)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Link object files to create the executable for release
$(BIN_DIR)/main_release: $(OBJS_RELEASE)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files for development
$(BUILD_DIR)/dev/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile source files to object files for release
$(BUILD_DIR)/release/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Apple Silicon debug
debug: $(BIN_DIR)/main_dev
	lldb -o 'run' $(BIN_DIR)/main_dev

# Run the program
run_dev: $(BIN_DIR)/main_dev
	./$(BIN_DIR)/main_dev

run_release: $(BIN_DIR)/main_release
	./$(BIN_DIR)/main_release

# Phony targets
.PHONY: all clean dev release debug run_dev run_release