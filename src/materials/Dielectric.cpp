#include "materials/Dielectric.h"

#include <cmath>

#include "core/Random.h"
#include "geometry/Hittable.h"

namespace auraray::materials {

Dielectric::Dielectric(double indexOfRefraction)
    : indexOfRefraction_(indexOfRefraction) {}

bool Dielectric::scatter(const core::Ray& incoming,
                         const geometry::HitRecord& hit,
                         core::Color& attenuation, core::Ray& scattered,
                         std::mt19937& random) const {
    attenuation = core::Color(1.0, 1.0, 1.0);
    const double refractionRatio =
        hit.frontFace ? (1.0 / indexOfRefraction_) : indexOfRefraction_;

    const core::Vec3 unitDirection = core::unitVector(incoming.direction());
    const double cosTheta =
        std::fmin(core::dot(-unitDirection, hit.normal), 1.0);
    const double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

    const bool cannotRefract = refractionRatio * sinTheta > 1.0;
    core::Vec3 direction;
    if (cannotRefract ||
        reflectance(cosTheta, refractionRatio) > core::randomDouble(random)) {
        direction = core::reflect(unitDirection, hit.normal);
    } else {
        direction = core::refract(unitDirection, hit.normal, refractionRatio);
    }

    scattered = core::Ray(hit.point, direction);
    return true;
}

double Dielectric::reflectance(double cosine, double refractionIndex) {
    double base = (1 - refractionIndex) / (1 + refractionIndex);
    base = base * base;
    return base + (1 - base) * std::pow((1 - cosine), 5);
}

}  // namespace auraray::materials
