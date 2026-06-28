#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace auraray::io {

struct SceneMetadata {
    std::string sceneName;
    std::filesystem::path outputPpm;
    std::filesystem::path outputPng;
    int imageWidth;
    int imageHeight;
    int samplesPerPixel;
    int maxDepth;
    long long renderTimeMs;
    int sphereCount;
    std::vector<std::string> materialsUsed;
    std::string cameraDescription;
};

struct FoveatedMetadata {
    std::string renderMode;
    std::string sceneName;
    std::filesystem::path outputPpm;
    std::filesystem::path outputPng;
    int imageWidth;
    int imageHeight;
    double gazeX;
    double gazeY;
    double fovealRadius;
    double transitionRadius;
    int samplesCenter;
    int samplesMiddle;
    int samplesPeriphery;
    long long estimatedTotalRays;
    long long renderTimeMs;
    std::vector<std::string> materialsUsed;
    std::string cameraDescription;
};

void writeMetadata(const std::filesystem::path& metadataPath,
                   const SceneMetadata& metadata);
void writeMetadata(const std::filesystem::path& metadataPath,
                   const FoveatedMetadata& metadata);

}  // namespace auraray::io
