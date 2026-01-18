CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
SDL_FLAGS = $(shell sdl2-config --cflags --libs)
VULKAN_FLAGS = -lvulkan

# Directories
SRC_DIR = src
SHADER_DIR = shaders
BUILD_DIR = build

# Files
SRC = $(SRC_DIR)/main.cpp
TARGET = $(BUILD_DIR)/main

# Shaders
VERT_SHADER = $(SHADER_DIR)/shader.vert
FRAG_SHADER = $(SHADER_DIR)/shader.frag
VERT_SPV = $(SHADER_DIR)/vert.spv
FRAG_SPV = $(SHADER_DIR)/frag.spv

all: $(VERT_SPV) $(FRAG_SPV) $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SDL_FLAGS) $(VULKAN_FLAGS)

$(VERT_SPV): $(VERT_SHADER)
	glslc $< -o $@

$(FRAG_SPV): $(FRAG_SHADER)
	glslc $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(SHADER_DIR)/*.spv

debug: CXXFLAGS += -g
debug: clean all

.PHONY: all run clean debug