CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2

BUILD_DIR := build
TARGET := $(BUILD_DIR)/auraray
SRC := src/main.cpp
FIRST_IMAGE_PPM := renders/first_image.ppm
FIRST_IMAGE_PNG := renders/first_image.png
FIRST_SPHERE_PPM := renders/first_sphere.ppm
FIRST_SPHERE_PNG := renders/first_sphere.png
ANTIALIAS_SPHERE_PPM := renders/antialias_sphere.ppm
ANTIALIAS_SPHERE_PNG := renders/antialias_sphere.png

.PHONY: all run png export clean

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

png export: run
	sips -s format png $(FIRST_IMAGE_PPM) --out $(FIRST_IMAGE_PNG)
	sips -s format png $(FIRST_SPHERE_PPM) --out $(FIRST_SPHERE_PNG)
	sips -s format png $(ANTIALIAS_SPHERE_PPM) --out $(ANTIALIAS_SPHERE_PNG)

clean:
	rm -rf $(BUILD_DIR)
