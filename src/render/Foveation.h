#pragma once

#include <filesystem>

namespace auraray::render {

bool renderFoveatedComparison(
    const std::filesystem::path& outputDirectory);

}  // namespace auraray::render
