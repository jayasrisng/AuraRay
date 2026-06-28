#include "core/Vec3.h"

#include <cmath>

namespace auraray::core {

Vec3::Vec3() : values_{0, 0, 0} {}

Vec3::Vec3(double x, double y, double z) : values_{x, y, z} {}

double Vec3::x() const { return values_[0]; }
double Vec3::y() const { return values_[1]; }
double Vec3::z() const { return values_[2]; }

Vec3 Vec3::operator-() const {
    return Vec3(-values_[0], -values_[1], -values_[2]);
}

double Vec3::operator[](int index) const { return values_[index]; }
double& Vec3::operator[](int index) { return values_[index]; }

Vec3& Vec3::operator+=(const Vec3& value) {
    values_[0] += value.values_[0];
    values_[1] += value.values_[1];
    values_[2] += value.values_[2];
    return *this;
}

Vec3& Vec3::operator*=(double scalar) {
    values_[0] *= scalar;
    values_[1] *= scalar;
    values_[2] *= scalar;
    return *this;
}

Vec3& Vec3::operator/=(double scalar) { return *this *= 1 / scalar; }

double Vec3::length() const { return std::sqrt(lengthSquared()); }

double Vec3::lengthSquared() const {
    return values_[0] * values_[0] + values_[1] * values_[1] +
           values_[2] * values_[2];
}

bool Vec3::nearZero() const {
    constexpr double threshold = 1e-8;
    return std::fabs(values_[0]) < threshold &&
           std::fabs(values_[1]) < threshold &&
           std::fabs(values_[2]) < threshold;
}

Vec3 operator+(const Vec3& left, const Vec3& right) {
    return Vec3(left[0] + right[0], left[1] + right[1], left[2] + right[2]);
}

Vec3 operator-(const Vec3& left, const Vec3& right) {
    return Vec3(left[0] - right[0], left[1] - right[1], left[2] - right[2]);
}

Vec3 operator*(const Vec3& left, const Vec3& right) {
    return Vec3(left[0] * right[0], left[1] * right[1], left[2] * right[2]);
}

Vec3 operator*(double scalar, const Vec3& value) {
    return Vec3(scalar * value[0], scalar * value[1], scalar * value[2]);
}

Vec3 operator*(const Vec3& value, double scalar) { return scalar * value; }

Vec3 operator/(const Vec3& value, double scalar) {
    return (1 / scalar) * value;
}

double dot(const Vec3& left, const Vec3& right) {
    return left[0] * right[0] + left[1] * right[1] + left[2] * right[2];
}

Vec3 cross(const Vec3& left, const Vec3& right) {
    return Vec3(left[1] * right[2] - left[2] * right[1],
                left[2] * right[0] - left[0] * right[2],
                left[0] * right[1] - left[1] * right[0]);
}

Vec3 unitVector(const Vec3& value) { return value / value.length(); }

Vec3 reflect(const Vec3& value, const Vec3& normal) {
    return value - 2 * dot(value, normal) * normal;
}

Vec3 refract(const Vec3& unitDirection, const Vec3& normal,
             double refractionRatio) {
    const double cosTheta = std::fmin(dot(-unitDirection, normal), 1.0);
    const Vec3 perpendicular =
        refractionRatio * (unitDirection + cosTheta * normal);
    const Vec3 parallel =
        -std::sqrt(std::fabs(1.0 - perpendicular.lengthSquared())) * normal;
    return perpendicular + parallel;
}

}  // namespace auraray::core
