#pragma once

#include "core/Vec3.h"

namespace auraray::core {

class Ray {
  public:
    Ray() = default;
    Ray(const Point3& origin, const Vec3& direction)
        : origin_(origin), direction_(direction) {}

    Point3 origin() const { return origin_; }
    Vec3 direction() const { return direction_; }
    Point3 at(double distance) const { return origin_ + distance * direction_; }

  private:
    Point3 origin_;
    Vec3 direction_;
};

}  // namespace auraray::core
