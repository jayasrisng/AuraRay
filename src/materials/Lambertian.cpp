#include "materials/Lambertian.h"

#include "core/Random.h"
#include "geometry/Hittable.h"

namespace auraray::materials {

Lambertian::Lambertian(const core::Color& albedo) : albedo_(albedo) {}

bool Lambertian::scatter(const core::Ray&, const geometry::HitRecord& hit,
                         core::Color& attenuation, core::Ray& scattered,
                         std::mt19937& random) const {
    core::Vec3 direction = hit.normal + core::randomUnitVector(random);
    if (direction.nearZero()) {
        direction = hit.normal;
    }

    scattered = core::Ray(hit.point, direction);
    attenuation = albedo_;
    return true;
}

}  // namespace auraray::materials
