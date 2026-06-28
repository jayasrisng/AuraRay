#include "render/Renderer.h"

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>

#include "core/Random.h"
#include "io/ImageWriter.h"
#include "io/MetadataWriter.h"
#include "materials/Material.h"
#include "render/Config.h"
#include "render/Foveation.h"
#include "render/Scene.h"

namespace auraray::render {
namespace {

double hitSimpleSphere(const core::Point3& center, double radius,
                       const core::Ray& ray) {
    const core::Vec3 offset = ray.origin() - center;
    const double a = core::dot(ray.direction(), ray.direction());
    const double b = 2.0 * core::dot(offset, ray.direction());
    const double c = core::dot(offset, offset) - radius * radius;
    const double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return -1.0;
    }
    return (-b - std::sqrt(discriminant)) / (2.0 * a);
}

core::Color simpleRayColor(const core::Ray& ray) {
    const double distance =
        hitSimpleSphere(core::Point3(0, 0, -1), 0.5, ray);
    if (distance > 0.0) {
        const core::Vec3 normal =
            core::unitVector(ray.at(distance) - core::Vec3(0, 0, -1));
        return 0.5 * core::Color(normal.x() + 1, normal.y() + 1,
                                 normal.z() + 1);
    }

    const core::Vec3 unitDirection = core::unitVector(ray.direction());
    const double blend = 0.5 * (unitDirection.y() + 1.0);
    return (1.0 - blend) * core::Color(1.0, 1.0, 1.0) +
           blend * core::Color(0.5, 0.7, 1.0);
}

}  // namespace

core::Color traceSceneRay(const core::Ray& ray,
                          const geometry::HittableList& world, int depth,
                          std::mt19937& random) {
    if (depth <= 0) {
        return core::Color(0, 0, 0);
    }

    geometry::HitRecord hit;
    if (world.hit(ray, 0.001, std::numeric_limits<double>::infinity(), hit)) {
        core::Ray scattered;
        core::Color attenuation;
        if (hit.material->scatter(ray, hit, attenuation, scattered, random)) {
            return attenuation *
                   traceSceneRay(scattered, world, depth - 1, random);
        }
        return core::Color(0, 0, 0);
    }

    const core::Vec3 unitDirection = core::unitVector(ray.direction());
    const double blend = 0.5 * (unitDirection.y() + 1.0);
    return (1.0 - blend) * core::Color(1.0, 1.0, 1.0) +
           blend * core::Color(0.58, 0.75, 1.0);
}

bool Renderer::renderFirstImage(
    const std::filesystem::path& outputPath) const {
    constexpr int height = 225;
    std::ofstream output;
    if (!io::openPpm(outputPath, kImageWidth, height, output)) {
        return false;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < kImageWidth; ++x) {
            const double horizontal =
                static_cast<double>(x) / (kImageWidth - 1);
            const double vertical = static_cast<double>(y) / (height - 1);
            io::writeColor(
                output,
                core::Color(horizontal, 1.0 - vertical, 96.0 / 255.999));
        }
    }

    std::cout << "Rendered " << outputPath << " (" << kImageWidth << "x"
              << height << ")\n";
    return true;
}

bool Renderer::renderFirstSphere(
    const std::filesystem::path& outputPath) const {
    const double viewportHeight = 2.0;
    const double viewportWidth = kAspectRatio * viewportHeight;
    const double focalLength = 1.0;

    const core::Point3 cameraOrigin(0, 0, 0);
    const core::Vec3 horizontal(viewportWidth, 0, 0);
    const core::Vec3 vertical(0, viewportHeight, 0);
    const core::Point3 lowerLeftCorner = cameraOrigin - horizontal / 2 -
                                         vertical / 2 -
                                         core::Vec3(0, 0, focalLength);

    std::ofstream output;
    if (!io::openPpm(outputPath, kImageWidth, kImageHeight, output)) {
        return false;
    }

    for (int y = kImageHeight - 1; y >= 0; --y) {
        for (int x = 0; x < kImageWidth; ++x) {
            const double u = static_cast<double>(x) / (kImageWidth - 1);
            const double v = static_cast<double>(y) / (kImageHeight - 1);
            const core::Ray ray(cameraOrigin,
                                lowerLeftCorner + u * horizontal +
                                    v * vertical - cameraOrigin);
            io::writeColor(output, simpleRayColor(ray));
        }
    }

    std::cout << "Rendered " << outputPath << " (" << kImageWidth << "x"
              << kImageHeight << ")\n";
    return true;
}

bool Renderer::renderAntialiasedSphere(
    const std::filesystem::path& outputPath) const {
    const double viewportHeight = 2.0;
    const double viewportWidth = kAspectRatio * viewportHeight;
    const double focalLength = 1.0;

    const core::Point3 cameraOrigin(0, 0, 0);
    const core::Vec3 horizontal(viewportWidth, 0, 0);
    const core::Vec3 vertical(0, viewportHeight, 0);
    const core::Point3 lowerLeftCorner = cameraOrigin - horizontal / 2 -
                                         vertical / 2 -
                                         core::Vec3(0, 0, focalLength);

    std::ofstream output;
    if (!io::openPpm(outputPath, kImageWidth, kImageHeight, output)) {
        return false;
    }

    std::mt19937 random(1337);
    for (int y = kImageHeight - 1; y >= 0; --y) {
        for (int x = 0; x < kImageWidth; ++x) {
            core::Color pixelColor(0, 0, 0);
            for (int sample = 0; sample < kSamplesPerPixel; ++sample) {
                const double u =
                    (static_cast<double>(x) + core::randomDouble(random)) /
                    (kImageWidth - 1);
                const double v =
                    (static_cast<double>(y) + core::randomDouble(random)) /
                    (kImageHeight - 1);
                const core::Ray ray(cameraOrigin,
                                    lowerLeftCorner + u * horizontal +
                                        v * vertical - cameraOrigin);
                pixelColor += simpleRayColor(ray);
            }
            io::writeColor(output, pixelColor, kSamplesPerPixel);
        }
    }

    std::cout << "Rendered " << outputPath << " (" << kImageWidth << "x"
              << kImageHeight << ", " << kSamplesPerPixel
              << " samples per pixel)\n";
    return true;
}

bool Renderer::renderMinimalScene(
    const std::filesystem::path& outputPath) const {
    const ScenePreset preset = makeMinimalScene();
    std::ofstream output;
    if (!io::openPpm(outputPath, kImageWidth, kImageHeight, output)) {
        return false;
    }

    std::mt19937 random(preset.seed);
    for (int y = kImageHeight - 1; y >= 0; --y) {
        for (int x = 0; x < kImageWidth; ++x) {
            core::Color pixelColor(0, 0, 0);
            for (int sample = 0; sample < kMinimalSceneSamplesPerPixel;
                 ++sample) {
                const double u =
                    (static_cast<double>(x) + core::randomDouble(random)) /
                    (kImageWidth - 1);
                const double v =
                    (static_cast<double>(y) + core::randomDouble(random)) /
                    (kImageHeight - 1);
                pixelColor += traceSceneRay(preset.camera.getRay(u, v),
                                            preset.world, kMaxRayDepth, random);
            }
            io::writeColor(output, pixelColor, kMinimalSceneSamplesPerPixel,
                           true);
        }
    }

    std::cout << "Rendered " << outputPath << " (" << kImageWidth << "x"
              << kImageHeight << ", " << kMinimalSceneSamplesPerPixel
              << " samples per pixel)\n";
    return true;
}

bool Renderer::renderPreset(const ScenePreset& preset,
                            const std::filesystem::path& outputDirectory) const {
    const std::filesystem::path outputPpm =
        outputDirectory / (preset.name + ".ppm");
    const std::filesystem::path outputPng =
        outputDirectory / (preset.name + ".png");
    const std::filesystem::path metadataPath =
        outputDirectory / (preset.name + ".json");

    std::ofstream output;
    if (!io::openPpm(outputPpm, kImageWidth, kImageHeight, output)) {
        return false;
    }

    std::mt19937 random(preset.seed);
    const auto start = std::chrono::steady_clock::now();
    for (int y = kImageHeight - 1; y >= 0; --y) {
        for (int x = 0; x < kImageWidth; ++x) {
            core::Color pixelColor(0, 0, 0);
            for (int sample = 0; sample < kPresetSamplesPerPixel; ++sample) {
                const double u =
                    (static_cast<double>(x) + core::randomDouble(random)) /
                    (kImageWidth - 1);
                const double v =
                    (static_cast<double>(y) + core::randomDouble(random)) /
                    (kImageHeight - 1);
                pixelColor += traceSceneRay(preset.camera.getRay(u, v),
                                            preset.world, kMaxRayDepth, random);
            }
            io::writeColor(output, pixelColor, kPresetSamplesPerPixel, true);
        }
    }

    const auto renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::steady_clock::now() - start)
                                .count();
    std::cout << "Rendered " << outputPpm << " (" << kImageWidth << "x"
              << kImageHeight << ", " << kPresetSamplesPerPixel
              << " samples per pixel)\n";

    io::writeMetadata(
        metadataPath,
        io::SceneMetadata{preset.name,
                          outputPpm,
                          outputPng,
                          kImageWidth,
                          kImageHeight,
                          kPresetSamplesPerPixel,
                          kMaxRayDepth,
                          renderTime,
                          static_cast<int>(preset.world.size()),
                          preset.materialsUsed,
                          preset.cameraDescription});
    return true;
}

bool Renderer::renderAll(
    const std::filesystem::path& outputDirectory) const {
    std::filesystem::create_directories(outputDirectory);
    return renderFirstImage(outputDirectory / "first_image.ppm") &&
           renderFirstSphere(outputDirectory / "first_sphere.ppm") &&
           renderAntialiasedSphere(outputDirectory / "antialias_sphere.ppm") &&
           renderMinimalScene(outputDirectory / "minimal_raytracer.ppm") &&
           renderPreset(makeGlassOrbsScene(), outputDirectory) &&
           renderPreset(makeXrLensDemoScene(), outputDirectory) &&
           renderPreset(makeWarmStudioSpheresScene(), outputDirectory) &&
           renderFoveatedComparison(outputDirectory);
}

}  // namespace auraray::render
