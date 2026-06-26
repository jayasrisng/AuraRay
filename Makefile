CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2
CMAKE ?= cmake

BUILD_DIR := build
TARGET := $(BUILD_DIR)/auraray
CMAKE_BUILD_DIR := $(BUILD_DIR)/cmake
CMAKE_BUILD_TYPE ?= Release
SRC := src/main.cpp
FIRST_IMAGE_PPM := renders/first_image.ppm
FIRST_IMAGE_PNG := renders/first_image.png
FIRST_SPHERE_PPM := renders/first_sphere.ppm
FIRST_SPHERE_PNG := renders/first_sphere.png
ANTIALIAS_SPHERE_PPM := renders/antialias_sphere.ppm
ANTIALIAS_SPHERE_PNG := renders/antialias_sphere.png
MINIMAL_RAYTRACER_PPM := renders/minimal_raytracer.ppm
MINIMAL_RAYTRACER_PNG := renders/minimal_raytracer.png
GLASS_ORBS_PPM := renders/glass_orbs.ppm
GLASS_ORBS_PNG := renders/glass_orbs.png
XR_LENS_DEMO_PPM := renders/xr_lens_demo.ppm
XR_LENS_DEMO_PNG := renders/xr_lens_demo.png
WARM_STUDIO_SPHERES_PPM := renders/warm_studio_spheres.ppm
WARM_STUDIO_SPHERES_PNG := renders/warm_studio_spheres.png
FOVEATED_FULL_PPM := renders/foveated_full.ppm
FOVEATED_FULL_PNG := renders/foveated_full.png
FOVEATED_LOW_PPM := renders/foveated_low.ppm
FOVEATED_LOW_PNG := renders/foveated_low.png
FOVEATED_GAZE_PPM := renders/foveated_gaze.ppm
FOVEATED_GAZE_PNG := renders/foveated_gaze.png
FOVEATED_OVERLAY_PPM := renders/foveated_overlay.ppm
FOVEATED_OVERLAY_PNG := renders/foveated_overlay.png

.PHONY: all run png export cmake-configure cmake-build cmake-run clean

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
	sips -s format png $(MINIMAL_RAYTRACER_PPM) --out $(MINIMAL_RAYTRACER_PNG)
	sips -s format png $(GLASS_ORBS_PPM) --out $(GLASS_ORBS_PNG)
	sips -s format png $(XR_LENS_DEMO_PPM) --out $(XR_LENS_DEMO_PNG)
	sips -s format png $(WARM_STUDIO_SPHERES_PPM) --out $(WARM_STUDIO_SPHERES_PNG)
	sips -s format png $(FOVEATED_FULL_PPM) --out $(FOVEATED_FULL_PNG)
	sips -s format png $(FOVEATED_LOW_PPM) --out $(FOVEATED_LOW_PNG)
	sips -s format png $(FOVEATED_GAZE_PPM) --out $(FOVEATED_GAZE_PNG)
	sips -s format png $(FOVEATED_OVERLAY_PPM) --out $(FOVEATED_OVERLAY_PNG)

cmake-configure:
	$(CMAKE) -S . -B $(CMAKE_BUILD_DIR) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)

cmake-build: cmake-configure
	$(CMAKE) --build $(CMAKE_BUILD_DIR) --config $(CMAKE_BUILD_TYPE)

cmake-run: cmake-build
	./$(CMAKE_BUILD_DIR)/auraray

clean:
	rm -rf $(BUILD_DIR)
