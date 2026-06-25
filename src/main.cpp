#include <filesystem>
#include <fstream>
#include <iostream>
#include <cmath>

class vec3 {
  public:
    vec3() : e{0, 0, 0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) { return *this *= 1 / t; }

    double length() const { return std::sqrt(length_squared()); }

    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

  private:
    double e[3];
};

using point3 = vec3;
using color = vec3;

vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

vec3 operator*(double t, const vec3& v) {
    return vec3(t * v[0], t * v[1], t * v[2]);
}

vec3 operator*(const vec3& v, double t) { return t * v; }

vec3 operator/(const vec3& v, double t) { return (1 / t) * v; }

double dot(const vec3& u, const vec3& v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

vec3 unit_vector(const vec3& v) { return v / v.length(); }

class ray {
  public:
    ray() = default;
    ray(const point3& origin, const vec3& direction)
        : origin_(origin), direction_(direction) {}

    point3 origin() const { return origin_; }
    vec3 direction() const { return direction_; }

    point3 at(double t) const { return origin_ + t * direction_; }

  private:
    point3 origin_;
    vec3 direction_;
};

void write_color(std::ostream& out, const color& pixel_color) {
    const int red = static_cast<int>(255.999 * pixel_color.x());
    const int green = static_cast<int>(255.999 * pixel_color.y());
    const int blue = static_cast<int>(255.999 * pixel_color.z());

    out << red << ' ' << green << ' ' << blue << '\n';
}

double hit_sphere(const point3& center, double radius, const ray& r) {
    const vec3 oc = r.origin() - center;
    const double a = dot(r.direction(), r.direction());
    const double b = 2.0 * dot(oc, r.direction());
    const double c = dot(oc, oc) - radius * radius;
    const double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return -1.0;
    }

    return (-b - std::sqrt(discriminant)) / (2.0 * a);
}

color ray_color(const ray& r) {
    const double t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if (t > 0.0) {
        const vec3 normal = unit_vector(r.at(t) - vec3(0, 0, -1));
        return 0.5 * color(normal.x() + 1, normal.y() + 1, normal.z() + 1);
    }

    const vec3 unit_direction = unit_vector(r.direction());
    const double a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

bool open_ppm(const std::filesystem::path& output_path, int image_width,
              int image_height, std::ofstream& out) {
    out.open(output_path);
    if (!out) {
        std::cerr << "Failed to open " << output_path << " for writing.\n";
        return false;
    }

    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    return true;
}

bool render_first_image(const std::filesystem::path& output_path) {
    const int image_width = 400;
    const int image_height = 225;

    std::ofstream out;
    if (!open_ppm(output_path, image_width, image_height, out)) {
        return false;
    }

    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x) {
            const double u = static_cast<double>(x) / (image_width - 1);
            const double v = static_cast<double>(y) / (image_height - 1);

            write_color(out, color(u, 1.0 - v, 96.0 / 255.999));
        }
    }

    std::cout << "Rendered " << output_path << " (" << image_width << "x"
              << image_height << ")\n";
    return true;
}

bool render_first_sphere(const std::filesystem::path& output_path) {
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    const double viewport_height = 2.0;
    const double viewport_width = aspect_ratio * viewport_height;
    const double focal_length = 1.0;

    const point3 camera_origin(0, 0, 0);
    const vec3 horizontal(viewport_width, 0, 0);
    const vec3 vertical(0, viewport_height, 0);
    const point3 lower_left_corner =
        camera_origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    std::ofstream out;
    if (!open_ppm(output_path, image_width, image_height, out)) {
        return false;
    }

    for (int y = image_height - 1; y >= 0; --y) {
        for (int x = 0; x < image_width; ++x) {
            const double u = static_cast<double>(x) / (image_width - 1);
            const double v = static_cast<double>(y) / (image_height - 1);
            const ray r(camera_origin,
                        lower_left_corner + u * horizontal + v * vertical -
                            camera_origin);

            write_color(out, ray_color(r));
        }
    }

    std::cout << "Rendered " << output_path << " (" << image_width << "x"
              << image_height << ")\n";
    return true;
}

int main() {
    const std::filesystem::path output_dir = "renders";
    std::filesystem::create_directories(output_dir);

    if (!render_first_image(output_dir / "first_image.ppm")) {
        return 1;
    }

    if (!render_first_sphere(output_dir / "first_sphere.ppm")) {
        return 1;
    }

    return 0;
}
