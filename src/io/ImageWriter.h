#pragma once

#include <filesystem>
#include <fstream>
#include <ostream>

#include "core/Vec3.h"

namespace auraray::io {

bool openPpm(const std::filesystem::path& outputPath, int width, int height,
             std::ofstream& output);
void writeColor(std::ostream& output, const core::Color& color,
                int samples = 1, bool gammaCorrect = false);

}  // namespace auraray::io
