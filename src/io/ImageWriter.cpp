#include "io/ImageWriter.h"

#include <cmath>
#include <iostream>

namespace auraray::io {
namespace {

double clamp(double value, double minimum, double maximum) {
    if (value < minimum) {
        return minimum;
    }
    if (value > maximum) {
        return maximum;
    }
    return value;
}

}  // namespace

bool openPpm(const std::filesystem::path& outputPath, int width, int height,
             std::ofstream& output) {
    output.open(outputPath);
    if (!output) {
        std::cerr << "Failed to open " << outputPath << " for writing.\n";
        return false;
    }

    output << "P3\n" << width << ' ' << height << "\n255\n";
    return true;
}

void writeColor(std::ostream& output, const core::Color& color, int samples,
                bool gammaCorrect) {
    const double scale = 1.0 / samples;
    double red = scale * color.x();
    double green = scale * color.y();
    double blue = scale * color.z();

    if (!gammaCorrect) {
        const int redByte = static_cast<int>(255.999 * red);
        const int greenByte = static_cast<int>(255.999 * green);
        const int blueByte = static_cast<int>(255.999 * blue);
        output << redByte << ' ' << greenByte << ' ' << blueByte << '\n';
        return;
    }

    red = std::sqrt(red);
    green = std::sqrt(green);
    blue = std::sqrt(blue);

    const int redByte = static_cast<int>(256 * clamp(red, 0.0, 0.999));
    const int greenByte = static_cast<int>(256 * clamp(green, 0.0, 0.999));
    const int blueByte = static_cast<int>(256 * clamp(blue, 0.0, 0.999));
    output << redByte << ' ' << greenByte << ' ' << blueByte << '\n';
}

}  // namespace auraray::io
