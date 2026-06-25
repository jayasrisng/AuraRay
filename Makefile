CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2

BUILD_DIR := build
TARGET := $(BUILD_DIR)/auraray
SRC := src/main.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

