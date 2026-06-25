CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2

BUILD_DIR := build
TARGET := $(BUILD_DIR)/auraray
SRC := src/main.cpp
PPM := renders/first_image.ppm
PNG := renders/first_image.png

.PHONY: all run png export clean

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

png export: run
	sips -s format png $(PPM) --out $(PNG)

clean:
	rm -rf $(BUILD_DIR)
