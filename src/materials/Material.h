#pragma once

#include <random>

#include "core/Ray.h"

namespace auraray::geometry {
struct HitRecord;
}

namespace auraray::materials {

class Material {
  public:
    virtual ~Material() = default;
    virtual bool scatter(const core::Ray& incoming,
                         const geometry::HitRecord& hit,
                         core::Color& attenuation, core::Ray& scattered,
                         std::mt19937& random) const = 0;
};

}  // namespace auraray::materials
