#include <filesystem>
#include <iostream>
#include <string>

#include "render/Renderer.h"

namespace {

void printUsage(const char* executable) {
    std::cout << "Usage: " << executable << " [--output-dir PATH]\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    std::filesystem::path outputDirectory = "renders";

    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--help" || argument == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        if (argument == "--output-dir" && index + 1 < argc) {
            outputDirectory = argv[++index];
            continue;
        }

        std::cerr << "Unknown or incomplete argument: " << argument << '\n';
        printUsage(argv[0]);
        return 2;
    }

    const auraray::render::Renderer renderer;
    return renderer.renderAll(outputDirectory) ? 0 : 1;
}
