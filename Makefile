# Compiler and flags
CC = gcc
CFLAGS_COMMON = -std=c18 -Wall -Wextra -Wpedantic -pthread
CFLAGS_DEV = -g -Werror -fsanitize=address
CFLAGS_RELEASE = -O2

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

BIN_DEV = $(BIN_DIR)/clabdev
BIN_RELEASE = $(BIN_DIR)/clab

# Find all .c files in SRC_DIR and subdirectories
SRCS = $(shell find $(SRC_DIR) -name '*.c')
# Create a list of .o files based on the .c files
OBJS_DEV = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/dev/%.o,$(SRCS))
OBJS_RELEASE = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/release/%.o,$(SRCS))

# Default target
all: dev release

# Development build
dev: CFLAGS=$(CFLAGS_COMMON) $(CFLAGS_DEV)
dev: $(BIN_DEV)

# Release build
release: CFLAGS=$(CFLAGS_COMMON) $(CFLAGS_RELEASE)
release: $(BIN_RELEASE)

# Link object files to create the executable for development
$(BIN_DEV): $(OBJS_DEV)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Link object files to create the executable for release
$(BIN_RELEASE): $(OBJS_RELEASE)
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
debug: $(BIN_DEV)
	lldb -o 'run' $(BIN_DEV)

# Run the program
runclabdev: $(BIN_DEV)
	./$(BIN_DEV) $(cmd1) $(cmd2)

runclab: $(BIN_RELEASE)
	./$(BIN_RELEASE) $(cmd1) $(cmd2)

# Phony targets
.PHONY: all clean dev release debug runclabdev runclab