#include "io/MetadataWriter.h"

#include <fstream>
#include <iostream>

namespace auraray::io {
namespace {

void writeStringArray(std::ofstream& output,
                      const std::vector<std::string>& values) {
    output << '[';
    for (std::size_t index = 0; index < values.size(); ++index) {
        output << '"' << values[index] << '"';
        if (index + 1 < values.size()) {
            output << ", ";
        }
    }
    output << ']';
}

std::ofstream openMetadata(const std::filesystem::path& metadataPath) {
    std::ofstream output(metadataPath);
    if (!output) {
        std::cerr << "Failed to open " << metadataPath << " for writing.\n";
    }
    return output;
}

}  // namespace

void writeMetadata(const std::filesystem::path& metadataPath,
                   const SceneMetadata& metadata) {
    if (std::filesystem::exists(metadataPath)) {
        return;
    }

    std::ofstream output = openMetadata(metadataPath);
    if (!output) {
        return;
    }

    output << "{\n";
    output << "  \"scene_name\": \"" << metadata.sceneName << "\",\n";
    output << "  \"output_ppm\": \"" << metadata.outputPpm.string()
           << "\",\n";
    output << "  \"output_png\": \"" << metadata.outputPng.string()
           << "\",\n";
    output << "  \"image_width\": " << metadata.imageWidth << ",\n";
    output << "  \"image_height\": " << metadata.imageHeight << ",\n";
    output << "  \"samples_per_pixel\": " << metadata.samplesPerPixel
           << ",\n";
    output << "  \"max_depth\": " << metadata.maxDepth << ",\n";
    output << "  \"render_time_ms\": " << metadata.renderTimeMs << ",\n";
    output << "  \"sphere_count\": " << metadata.sphereCount << ",\n";
    output << "  \"materials_used\": ";
    writeStringArray(output, metadata.materialsUsed);
    output << ",\n";
    output << "  \"camera_description\": \""
           << metadata.cameraDescription << "\"\n";
    output << "}\n";
}

void writeMetadata(const std::filesystem::path& metadataPath,
                   const FoveatedMetadata& metadata) {
    if (std::filesystem::exists(metadataPath)) {
        return;
    }

    std::ofstream output = openMetadata(metadataPath);
    if (!output) {
        return;
    }

    output << "{\n";
    output << "  \"render_mode\": \"" << metadata.renderMode << "\",\n";
    output << "  \"scene_name\": \"" << metadata.sceneName << "\",\n";
    output << "  \"output_ppm\": \"" << metadata.outputPpm.string()
           << "\",\n";
    output << "  \"output_png\": \"" << metadata.outputPng.string()
           << "\",\n";
    output << "  \"image_width\": " << metadata.imageWidth << ",\n";
    output << "  \"image_height\": " << metadata.imageHeight << ",\n";
    output << "  \"gaze_x\": " << metadata.gazeX << ",\n";
    output << "  \"gaze_y\": " << metadata.gazeY << ",\n";
    output << "  \"foveal_radius\": " << metadata.fovealRadius << ",\n";
    output << "  \"transition_radius\": " << metadata.transitionRadius
           << ",\n";
    output << "  \"samples_center\": " << metadata.samplesCenter << ",\n";
    output << "  \"samples_middle\": " << metadata.samplesMiddle << ",\n";
    output << "  \"samples_periphery\": " << metadata.samplesPeriphery
           << ",\n";
    output << "  \"estimated_total_rays\": "
           << metadata.estimatedTotalRays << ",\n";
    output << "  \"render_time_ms\": " << metadata.renderTimeMs << ",\n";
    output << "  \"materials_used\": ";
    writeStringArray(output, metadata.materialsUsed);
    output << ",\n";
    output << "  \"camera_description\": \""
           << metadata.cameraDescription << "\"\n";
    output << "}\n";
}

}  // namespace auraray::io
