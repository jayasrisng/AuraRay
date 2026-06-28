#pragma once

#include <string>
#include <vector>

#include "core/Camera.h"
#include "geometry/HittableList.h"

namespace auraray::render {

struct ScenePreset {
    std::string name;
    geometry::HittableList world;
    core::Camera camera;
    std::vector<std::string> materialsUsed;
    std::string cameraDescription;
    unsigned int seed;
};

ScenePreset makeMinimalScene();
ScenePreset makeGlassOrbsScene();
ScenePreset makeXrLensDemoScene();
ScenePreset makeWarmStudioSpheresScene();

}  // namespace auraray::render
