#pragma once

#include <filesystem>
#include <random>

#include "core/Ray.h"
#include "geometry/HittableList.h"

namespace auraray::render {

struct ScenePreset;

core::Color traceSceneRay(const core::Ray& ray,
                          const geometry::HittableList& world, int depth,
                          std::mt19937& random);

class Renderer {
  public:
    bool renderAll(const std::filesystem::path& outputDirectory) const;

  private:
    bool renderFirstImage(const std::filesystem::path& outputPath) const;
    bool renderFirstSphere(const std::filesystem::path& outputPath) const;
    bool renderAntialiasedSphere(
        const std::filesystem::path& outputPath) const;
    bool renderMinimalScene(const std::filesystem::path& outputPath) const;
    bool renderPreset(const ScenePreset& preset,
                      const std::filesystem::path& outputDirectory) const;
};

}  // namespace auraray::render
