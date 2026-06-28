#include "geometry/Hittable.h"

namespace auraray::geometry {

void HitRecord::setFaceNormal(const core::Ray& ray,
                              const core::Vec3& outwardNormal) {
    frontFace = core::dot(ray.direction(), outwardNormal) < 0;
    normal = frontFace ? outwardNormal : -outwardNormal;
}

}  // namespace auraray::geometry
