#pragma once

#include "core/Ray.h"

namespace auraray::core {

class Camera {
  public:
    Camera(Point3 lookFrom, Point3 lookAt, Vec3 viewUp,
           double verticalFieldOfView, double aspectRatio);

    Ray getRay(double horizontal, double vertical) const;

  private:
    Point3 origin_;
    Point3 lowerLeftCorner_;
    Vec3 horizontal_;
    Vec3 vertical_;
};

}  // namespace auraray::core
