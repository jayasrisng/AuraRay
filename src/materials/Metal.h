#pragma once

#include "materials/Material.h"

namespace auraray::materials {

class Metal final : public Material {
  public:
    Metal(const core::Color& albedo, double fuzz);
    bool scatter(const core::Ray& incoming, const geometry::HitRecord& hit,
                 core::Color& attenuation, core::Ray& scattered,
                 std::mt19937& random) const override;

  private:
    core::Color albedo_;
    double fuzz_;
};

}  // namespace auraray::materials
