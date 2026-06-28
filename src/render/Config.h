#pragma once

namespace auraray::render {

inline constexpr int kImageWidth = 400;
inline constexpr double kAspectRatio = 16.0 / 9.0;
inline constexpr int kImageHeight =
    static_cast<int>(kImageWidth / kAspectRatio);
inline constexpr int kSamplesPerPixel = 25;
inline constexpr int kMinimalSceneSamplesPerPixel = 100;
inline constexpr int kPresetSamplesPerPixel = 80;
inline constexpr int kMaxRayDepth = 8;

}  // namespace auraray::render
