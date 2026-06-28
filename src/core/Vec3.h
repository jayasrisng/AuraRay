#pragma once

namespace auraray::core {

class Vec3 {
  public:
    Vec3();
    Vec3(double x, double y, double z);

    double x() const;
    double y() const;
    double z() const;

    Vec3 operator-() const;
    double operator[](int index) const;
    double& operator[](int index);
    Vec3& operator+=(const Vec3& value);
    Vec3& operator*=(double scalar);
    Vec3& operator/=(double scalar);

    double length() const;
    double lengthSquared() const;
    bool nearZero() const;

  private:
    double values_[3];
};

using Point3 = Vec3;
using Color = Vec3;

Vec3 operator+(const Vec3& left, const Vec3& right);
Vec3 operator-(const Vec3& left, const Vec3& right);
Vec3 operator*(const Vec3& left, const Vec3& right);
Vec3 operator*(double scalar, const Vec3& value);
Vec3 operator*(const Vec3& value, double scalar);
Vec3 operator/(const Vec3& value, double scalar);

double dot(const Vec3& left, const Vec3& right);
Vec3 cross(const Vec3& left, const Vec3& right);
Vec3 unitVector(const Vec3& value);
Vec3 reflect(const Vec3& value, const Vec3& normal);
Vec3 refract(const Vec3& unitDirection, const Vec3& normal,
             double refractionRatio);

}  // namespace auraray::core
