#pragma once

#include <memory>

#include "geometry/Hittable.h"

namespace auraray::geometry {

class Sphere final : public Hittable {
  public:
    Sphere(const core::Point3& center, double radius,
           std::shared_ptr<materials::Material> material);

    bool hit(const core::Ray& ray, double minimumDistance,
             double maximumDistance, HitRecord& record) const override;

  private:
    core::Point3 center_;
    double radius_;
    std::shared_ptr<materials::Material> material_;
};

}  // namespace auraray::geometry
