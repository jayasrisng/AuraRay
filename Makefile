CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2
CMAKE ?= cmake
SWIFT ?= /usr/bin/swift
UNITY ?= /Applications/Unity/Hub/Editor/6000.3.17f1/Unity.app/Contents/MacOS/Unity

BUILD_DIR := build
TARGET := $(BUILD_DIR)/auraray
CMAKE_BUILD_DIR := $(BUILD_DIR)/cmake
CMAKE_BUILD_TYPE ?= Release
UNITY_PROJECT := $(CURDIR)/unity/AuraRayViewer
DEMO_FRAME_DIR ?= /tmp/AuraRayDemoFrames
MEDIA_DIR := docs/media
SWIFT_CACHE_DIR ?= /tmp/auraray-swift-cache
SRC := $(wildcard src/*.cpp src/core/*.cpp src/geometry/*.cpp src/io/*.cpp src/materials/*.cpp src/render/*.cpp)
HEADERS := $(wildcard src/core/*.h src/geometry/*.h src/io/*.h src/materials/*.h src/render/*.h)
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

.PHONY: all run verify png export cmake-configure cmake-build cmake-run comparison-image demo-frames demo-media clean

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -Isrc $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

verify: $(TARGET)
	sh scripts/verify_render_reproducibility.sh $(TARGET)

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

comparison-image:
	mkdir -p $(MEDIA_DIR) $(SWIFT_CACHE_DIR)
	CLANG_MODULE_CACHE_PATH=$(SWIFT_CACHE_DIR) SWIFT_MODULECACHE_PATH=$(SWIFT_CACHE_DIR) $(SWIFT) scripts/generate_demo_media.swift comparison $(CURDIR) $(CURDIR)/$(MEDIA_DIR)/foveated_comparison.png

demo-frames:
	AURARAY_DEMO_FRAME_DIR=$(DEMO_FRAME_DIR) "$(UNITY)" -batchmode -quit -projectPath "$(UNITY_PROJECT)" -executeMethod AuraRayEditor.AuraRayDemoCapture.Capture -logFile /tmp/auraray_demo_capture.log

demo-media: comparison-image demo-frames
	CLANG_MODULE_CACHE_PATH=$(SWIFT_CACHE_DIR) SWIFT_MODULECACHE_PATH=$(SWIFT_CACHE_DIR) $(SWIFT) scripts/generate_demo_media.swift gif $(DEMO_FRAME_DIR) $(CURDIR)/$(MEDIA_DIR)/auraray_demo.gif

clean:
	rm -rf $(BUILD_DIR)
