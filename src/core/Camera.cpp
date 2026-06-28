#include "core/Camera.h"

#include <cmath>

namespace auraray::core {

Camera::Camera(Point3 lookFrom, Point3 lookAt, Vec3 viewUp,
               double verticalFieldOfView, double aspectRatio) {
    const double theta =
        verticalFieldOfView * 3.1415926535897932385 / 180.0;
    const double halfHeight = std::tan(theta / 2);
    const double viewportHeight = 2.0 * halfHeight;
    const double viewportWidth = aspectRatio * viewportHeight;

    const Vec3 backward = unitVector(lookFrom - lookAt);
    const Vec3 right = unitVector(cross(viewUp, backward));
    const Vec3 up = cross(backward, right);

    origin_ = lookFrom;
    horizontal_ = viewportWidth * right;
    vertical_ = viewportHeight * up;
    lowerLeftCorner_ =
        origin_ - horizontal_ / 2 - vertical_ / 2 - backward;
}

Ray Camera::getRay(double horizontal, double vertical) const {
    return Ray(origin_, lowerLeftCorner_ + horizontal * horizontal_ +
                            vertical * vertical_ - origin_);
}

}  // namespace auraray::core
