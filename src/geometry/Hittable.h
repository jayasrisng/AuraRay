#pragma once

#include <memory>

#include "core/Ray.h"

namespace auraray::materials {
class Material;
}

namespace auraray::geometry {

struct HitRecord {
    core::Point3 point;
    core::Vec3 normal;
    std::shared_ptr<materials::Material> material;
    double distance = 0;
    bool frontFace = false;

    void setFaceNormal(const core::Ray& ray, const core::Vec3& outwardNormal);
};

class Hittable {
  public:
    virtual ~Hittable() = default;
    virtual bool hit(const core::Ray& ray, double minimumDistance,
                     double maximumDistance, HitRecord& record) const = 0;
};

}  // namespace auraray::geometry
