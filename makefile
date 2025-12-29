CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2
SDL_FLAGS = $(shell sdl2-config --cflags --libs)

TARGET = main
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SDL_FLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

debug: CXXFLAGS += -g
debug: clean all

.PHONY: all run clean debug