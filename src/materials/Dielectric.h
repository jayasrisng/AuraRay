#pragma once

#include "materials/Material.h"

namespace auraray::materials {

class Dielectric final : public Material {
  public:
    explicit Dielectric(double indexOfRefraction);
    bool scatter(const core::Ray& incoming, const geometry::HitRecord& hit,
                 core::Color& attenuation, core::Ray& scattered,
                 std::mt19937& random) const override;

  private:
    static double reflectance(double cosine, double refractionIndex);
    double indexOfRefraction_;
};

}  // namespace auraray::materials
