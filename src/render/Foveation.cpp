#include "render/Foveation.h"

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "core/Random.h"
#include "io/ImageWriter.h"
#include "io/MetadataWriter.h"
#include "render/Config.h"
#include "render/Renderer.h"
#include "render/Scene.h"

namespace auraray::render {
namespace {

struct FoveationSettings {
    std::string renderMode;
    std::filesystem::path outputPpm;
    std::filesystem::path outputPng;
    std::filesystem::path metadataPath;
    int centerSamples;
    int middleSamples;
    int peripherySamples;
    unsigned int seed;
};

double foveatedDistance(double x, double y, double gazeX, double gazeY) {
    const double dx = (x - gazeX) * kAspectRatio;
    const double dy = y - gazeY;
    return std::sqrt(dx * dx + dy * dy);
}

int samplesForPixel(const FoveationSettings& settings, double distance,
                    double fovealRadius, double transitionRadius) {
    if (distance <= fovealRadius) {
        return settings.centerSamples;
    }
    if (distance <= transitionRadius) {
        return settings.middleSamples;
    }
    return settings.peripherySamples;
}

void writeFoveatedMetadata(const FoveationSettings& settings,
                           const ScenePreset& preset, double gazeX,
                           double gazeY, double fovealRadius,
                           double transitionRadius,
                           long long estimatedTotalRays,
                           long long renderTimeMs) {
    io::writeMetadata(
        settings.metadataPath,
        io::FoveatedMetadata{settings.renderMode,
                             preset.name,
                             settings.outputPpm,
                             settings.outputPng,
                             kImageWidth,
                             kImageHeight,
                             gazeX,
                             gazeY,
                             fovealRadius,
                             transitionRadius,
                             settings.centerSamples,
                             settings.middleSamples,
                             settings.peripherySamples,
                             estimatedTotalRays,
                             renderTimeMs,
                             preset.materialsUsed,
                             preset.cameraDescription});
}

bool renderFoveatedMode(const ScenePreset& preset,
                        const FoveationSettings& settings, double gazeX,
                        double gazeY, double fovealRadius,
                        double transitionRadius) {
    std::ofstream output;
    if (!io::openPpm(settings.outputPpm, kImageWidth, kImageHeight, output)) {
        return false;
    }

    std::mt19937 random(settings.seed);
    long long estimatedTotalRays = 0;
    const auto start = std::chrono::steady_clock::now();

    for (int y = kImageHeight - 1; y >= 0; --y) {
        for (int x = 0; x < kImageWidth; ++x) {
            const double normalizedX =
                static_cast<double>(x) / (kImageWidth - 1);
            const double normalizedY =
                static_cast<double>(y) / (kImageHeight - 1);
            const double distance =
                foveatedDistance(normalizedX, normalizedY, gazeX, gazeY);
            const int pixelSamples = samplesForPixel(
                settings, distance, fovealRadius, transitionRadius);

            core::Color pixelColor(0, 0, 0);
            for (int sample = 0; sample < pixelSamples; ++sample) {
                const double u =
                    (static_cast<double>(x) + core::randomDouble(random)) /
                    (kImageWidth - 1);
                const double v =
                    (static_cast<double>(y) + core::randomDouble(random)) /
                    (kImageHeight - 1);
                pixelColor += traceSceneRay(preset.camera.getRay(u, v),
                                            preset.world, kMaxRayDepth, random);
            }

            estimatedTotalRays += pixelSamples;
            io::writeColor(output, pixelColor, pixelSamples, true);
        }
    }

    const auto renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::steady_clock::now() - start)
                                .count();
    std::cout << "Rendered " << settings.outputPpm << " (" << kImageWidth
              << "x" << kImageHeight << ", " << settings.renderMode << ")\n";
    writeFoveatedMetadata(settings, preset, gazeX, gazeY, fovealRadius,
                          transitionRadius, estimatedTotalRays, renderTime);
    return true;
}

bool renderFoveatedOverlay(const std::filesystem::path& outputDirectory,
                           double gazeX, double gazeY, double fovealRadius,
                           double transitionRadius) {
    const std::filesystem::path outputPpm =
        outputDirectory / "foveated_overlay.ppm";
    const std::filesystem::path outputPng =
        outputDirectory / "foveated_overlay.png";
    const std::filesystem::path metadataPath =
        outputDirectory / "foveated_overlay.json";

    std::ofstream output;
    if (!io::openPpm(outputPpm, kImageWidth, kImageHeight, output)) {
        return false;
    }

    long long estimatedTotalRays = 0;
    const auto start = std::chrono::steady_clock::now();
    for (int y = kImageHeight - 1; y >= 0; --y) {
        for (int x = 0; x < kImageWidth; ++x) {
            const double normalizedX =
                static_cast<double>(x) / (kImageWidth - 1);
            const double normalizedY =
                static_cast<double>(y) / (kImageHeight - 1);
            const double distance =
                foveatedDistance(normalizedX, normalizedY, gazeX, gazeY);

            core::Color pixelColor(0.08, 0.10, 0.14);
            if (distance <= fovealRadius) {
                pixelColor = core::Color(0.15, 0.55, 1.0);
            } else if (distance <= transitionRadius) {
                pixelColor = core::Color(0.08, 0.28, 0.48);
            }

            const bool gazeDot = distance < 0.018;
            const bool fovealRing =
                std::fabs(distance - fovealRadius) < 0.006;
            const bool transitionRing =
                std::fabs(distance - transitionRadius) < 0.006;
            if (transitionRing) {
                pixelColor = core::Color(0.85, 0.95, 1.0);
            }
            if (fovealRing) {
                pixelColor = core::Color(1.0, 1.0, 1.0);
            }
            if (gazeDot) {
                pixelColor = core::Color(1.0, 0.18, 0.62);
            }
            io::writeColor(output, pixelColor);
        }
    }

    const auto renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::steady_clock::now() - start)
                                .count();
    const FoveationSettings overlaySettings{
        "overlay", outputPpm, outputPng, metadataPath, 64, 24, 8, 0};
    io::writeMetadata(
        metadataPath,
        io::FoveatedMetadata{
            overlaySettings.renderMode,
            "xr_lens_demo",
            outputPpm,
            outputPng,
            kImageWidth,
            kImageHeight,
            gazeX,
            gazeY,
            fovealRadius,
            transitionRadius,
            overlaySettings.centerSamples,
            overlaySettings.middleSamples,
            overlaySettings.peripherySamples,
            estimatedTotalRays,
            renderTime,
            {"overlay"},
            "Generated gaze point and foveal ring visualization for the foveated demo."});

    std::cout << "Rendered " << outputPpm << " (" << kImageWidth << "x"
              << kImageHeight << ", overlay)\n";
    return true;
}

}  // namespace

bool renderFoveatedComparison(
    const std::filesystem::path& outputDirectory) {
    const ScenePreset preset = makeXrLensDemoScene();
    constexpr double gazeX = 0.5;
    constexpr double gazeY = 0.5;
    constexpr double fovealRadius = 0.18;
    constexpr double transitionRadius = 0.36;

    const FoveationSettings full{
        "full_quality", outputDirectory / "foveated_full.ppm",
        outputDirectory / "foveated_full.png",
        outputDirectory / "foveated_full.json", 64, 64, 64, 4101};
    const FoveationSettings low{
        "low_quality", outputDirectory / "foveated_low.ppm",
        outputDirectory / "foveated_low.png",
        outputDirectory / "foveated_low.json", 8, 8, 8, 4102};
    const FoveationSettings gaze{
        "gaze_aware", outputDirectory / "foveated_gaze.ppm",
        outputDirectory / "foveated_gaze.png",
        outputDirectory / "foveated_gaze.json", 64, 24, 8, 4103};

    return renderFoveatedMode(preset, full, gazeX, gazeY, fovealRadius,
                              transitionRadius) &&
           renderFoveatedMode(preset, low, gazeX, gazeY, fovealRadius,
                              transitionRadius) &&
           renderFoveatedMode(preset, gaze, gazeX, gazeY, fovealRadius,
                              transitionRadius) &&
           renderFoveatedOverlay(outputDirectory, gazeX, gazeY, fovealRadius,
                                  transitionRadius);
}

}  // namespace auraray::render
