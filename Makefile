# Opxium Game Engine Makefile
# ============================

# Compiler settings
CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -std=c11
CFLAGS += -I./include
CFLAGS += $(shell pkg-config --cflags glfw3 vulkan)

LDFLAGS := $(shell pkg-config --libs glfw3 vulkan)
LDFLAGS += -lm

# Build modes
DEBUG_FLAGS := -g -O0 -DDEBUG
RELEASE_FLAGS := -O2 -DNDEBUG

# Directories
SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
SHADER_DIR := shaders
SHADER_BUILD_DIR := $(BUILD_DIR)/shaders

# Source files
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Shader files
VERT_SHADERS := $(wildcard $(SHADER_DIR)/*.vert)
FRAG_SHADERS := $(wildcard $(SHADER_DIR)/*.frag)
SPV_SHADERS := $(patsubst $(SHADER_DIR)/%.vert,$(SHADER_BUILD_DIR)/%.vert.spv,$(VERT_SHADERS))
SPV_SHADERS += $(patsubst $(SHADER_DIR)/%.frag,$(SHADER_BUILD_DIR)/%.frag.spv,$(FRAG_SHADERS))

# Output
TARGET := $(BIN_DIR)/opxium

# Default target
.PHONY: all
all: debug

# Debug build
.PHONY: debug
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)
	@echo "[DONE] Debug build complete: $(TARGET)"

# Release build
.PHONY: release
release: CFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)
	@echo "[DONE] Release build complete: $(TARGET)"

# Main executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	@echo "[LINK] $@"
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Directories
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(SHADER_BUILD_DIR):
	@mkdir -p $(SHADER_BUILD_DIR)

# Shader compilation
.PHONY: shaders
shaders: $(SHADER_BUILD_DIR) $(SPV_SHADERS)

$(SHADER_BUILD_DIR)/%.vert.spv: $(SHADER_DIR)/%.vert | $(SHADER_BUILD_DIR)
	@echo "[SHADER] $<"
	@glslc $< -o $@

$(SHADER_BUILD_DIR)/%.frag.spv: $(SHADER_DIR)/%.frag | $(SHADER_BUILD_DIR)
	@echo "[SHADER] $<"
	@glslc $< -o $@

# Run the application
.PHONY: run
run: debug
	@echo "[RUN] $(TARGET)"
	@./$(TARGET)

# Clean build artifacts
.PHONY: clean
clean:
	@echo "[CLEAN] Removing build directory"
	@rm -rf $(BUILD_DIR)

# Rebuild from scratch
.PHONY: rebuild
rebuild: clean all

# Install dependencies (Ubuntu/Debian)
.PHONY: deps
deps:
	@echo "[DEPS] Installing dependencies..."
	sudo apt-get update
	sudo apt-get install -y \
		libglfw3-dev \
		libvulkan-dev \
		vulkan-validationlayers-dev \
		glslc

# Format source code
.PHONY: format
format:
	@echo "[FORMAT] Formatting source files..."
	@find $(SRC_DIR) include -name '*.c' -o -name '*.h' | xargs clang-format -i

# Print help
.PHONY: help
help:
	@echo "Opxium Engine Build System"
	@echo "=========================="
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build debug version (default)"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized version"
	@echo "  shaders   - Compile GLSL shaders to SPIR-V"
	@echo "  run       - Build and run the engine"
	@echo "  clean     - Remove build artifacts"
	@echo "  rebuild   - Clean and rebuild"
	@echo "  deps      - Install dependencies (apt)"
	@echo "  format    - Format source code"
	@echo "  help      - Show this help"

# Dependency tracking
-include $(OBJS:.o=.d)
