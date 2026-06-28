#include "materials/Metal.h"

#include "core/Random.h"
#include "geometry/Hittable.h"

namespace auraray::materials {

Metal::Metal(const core::Color& albedo, double fuzz)
    : albedo_(albedo), fuzz_(fuzz < 1 ? fuzz : 1) {}

bool Metal::scatter(const core::Ray& incoming,
                    const geometry::HitRecord& hit,
                    core::Color& attenuation, core::Ray& scattered,
                    std::mt19937& random) const {
    const core::Vec3 reflected =
        core::reflect(core::unitVector(incoming.direction()), hit.normal);
    scattered = core::Ray(
        hit.point, reflected + fuzz_ * core::randomInUnitSphere(random));
    attenuation = albedo_;
    return core::dot(scattered.direction(), hit.normal) > 0;
}

}  // namespace auraray::materials
