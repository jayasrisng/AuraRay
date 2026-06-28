#include "geometry/Sphere.h"

#include <cmath>
#include <utility>

namespace auraray::geometry {

Sphere::Sphere(const core::Point3& center, double radius,
               std::shared_ptr<materials::Material> material)
    : center_(center), radius_(radius), material_(std::move(material)) {}

bool Sphere::hit(const core::Ray& ray, double minimumDistance,
                 double maximumDistance, HitRecord& record) const {
    const core::Vec3 offset = ray.origin() - center_;
    const double directionLength = ray.direction().lengthSquared();
    const double halfB = core::dot(offset, ray.direction());
    const double c = offset.lengthSquared() - radius_ * radius_;
    const double discriminant = halfB * halfB - directionLength * c;

    if (discriminant < 0) {
        return false;
    }

    const double squareRoot = std::sqrt(discriminant);
    double root = (-halfB - squareRoot) / directionLength;
    if (root < minimumDistance || maximumDistance < root) {
        root = (-halfB + squareRoot) / directionLength;
        if (root < minimumDistance || maximumDistance < root) {
            return false;
        }
    }

    record.distance = root;
    record.point = ray.at(record.distance);
    const core::Vec3 outwardNormal = (record.point - center_) / radius_;
    record.setFaceNormal(ray, outwardNormal);
    record.material = material_;
    return true;
}

}  // namespace auraray::geometry
