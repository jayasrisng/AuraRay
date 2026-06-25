#include <filesystem>
#include <fstream>
#include <iostream>

int main() {
    const int image_width = 400;
    const int image_height = 225;
    const std::filesystem::path output_dir = "renders";
    const std::filesystem::path output_path = output_dir / "first_image.ppm";

    std::filesystem::create_directories(output_dir);

    std::ofstream out(output_path);
    if (!out) {
        std::cerr << "Failed to open " << output_path << " for writing.\n";
        return 1;
    }

    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x) {
            const double u = static_cast<double>(x) / (image_width - 1);
            const double v = static_cast<double>(y) / (image_height - 1);

            const int red = static_cast<int>(255.999 * u);
            const int green = static_cast<int>(255.999 * (1.0 - v));
            const int blue = 96;

            out << red << ' ' << green << ' ' << blue << '\n';
        }
    }

    std::cout << "Rendered " << output_path << " (" << image_width << "x"
              << image_height << ")\n";
    return 0;
}

