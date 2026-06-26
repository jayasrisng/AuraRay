#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <vector>

constexpr int image_width = 400;
constexpr double aspect_ratio = 16.0 / 9.0;
constexpr int samples_per_pixel = 25;
constexpr int minimal_scene_samples_per_pixel = 100;
constexpr int max_ray_depth = 8;

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

    bool near_zero() const {
        const double s = 1e-8;
        return std::fabs(e[0]) < s && std::fabs(e[1]) < s &&
               std::fabs(e[2]) < s;
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

vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u[1] * v[2] - u[2] * v[1],
                u[2] * v[0] - u[0] * v[2],
                u[0] * v[1] - u[1] * v[0]);
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

double random_double(std::mt19937& rng) {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(rng);
}

double random_double(double min, double max, std::mt19937& rng) {
    return min + (max - min) * random_double(rng);
}

vec3 random_vec3(double min, double max, std::mt19937& rng) {
    return vec3(random_double(min, max, rng), random_double(min, max, rng),
                random_double(min, max, rng));
}

vec3 random_in_unit_sphere(std::mt19937& rng) {
    while (true) {
        const vec3 p = random_vec3(-1, 1, rng);
        if (p.length_squared() < 1) {
            return p;
        }
    }
}

vec3 random_unit_vector(std::mt19937& rng) {
    return unit_vector(random_in_unit_sphere(rng));
}

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    const double cos_theta = std::fmin(dot(-uv, n), 1.0);
    const vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    const vec3 r_out_parallel =
        -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

double clamp(double x, double min, double max) {
    if (x < min) {
        return min;
    }
    if (x > max) {
        return max;
    }
    return x;
}

void write_color(std::ostream& out, const color& pixel_color,
                 int samples = 1, bool gamma_correct = false) {
    const double scale = 1.0 / samples;
    double red = scale * pixel_color.x();
    double green = scale * pixel_color.y();
    double blue = scale * pixel_color.z();

    if (!gamma_correct) {
        const int red_byte = static_cast<int>(255.999 * red);
        const int green_byte = static_cast<int>(255.999 * green);
        const int blue_byte = static_cast<int>(255.999 * blue);
        out << red_byte << ' ' << green_byte << ' ' << blue_byte << '\n';
        return;
    }

    red = std::sqrt(red);
    green = std::sqrt(green);
    blue = std::sqrt(blue);

    const int red_byte = static_cast<int>(256 * clamp(red, 0.0, 0.999));
    const int green_byte = static_cast<int>(256 * clamp(green, 0.0, 0.999));
    const int blue_byte = static_cast<int>(256 * clamp(blue, 0.0, 0.999));

    out << red_byte << ' ' << green_byte << ' ' << blue_byte << '\n';
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

class material;

struct hit_record {
    point3 p;
    vec3 normal;
    std::shared_ptr<material> mat;
    double t = 0;
    bool front_face = false;

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class material {
  public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec,
                         color& attenuation, ray& scattered,
                         std::mt19937& rng) const = 0;
};

class lambertian : public material {
  public:
    explicit lambertian(const color& albedo) : albedo_(albedo) {}

    bool scatter(const ray&, const hit_record& rec, color& attenuation,
                 ray& scattered, std::mt19937& rng) const override {
        vec3 scatter_direction = rec.normal + random_unit_vector(rng);
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo_;
        return true;
    }

  private:
    color albedo_;
};

class metal : public material {
  public:
    metal(const color& albedo, double fuzz)
        : albedo_(albedo), fuzz_(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation,
                 ray& scattered, std::mt19937& rng) const override {
        const vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz_ * random_in_unit_sphere(rng));
        attenuation = albedo_;
        return dot(scattered.direction(), rec.normal) > 0;
    }

  private:
    color albedo_;
    double fuzz_;
};

class dielectric : public material {
  public:
    explicit dielectric(double index_of_refraction)
        : index_of_refraction_(index_of_refraction) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation,
                 ray& scattered, std::mt19937& rng) const override {
        attenuation = color(1.0, 1.0, 1.0);
        const double refraction_ratio =
            rec.front_face ? (1.0 / index_of_refraction_) : index_of_refraction_;

        const vec3 unit_direction = unit_vector(r_in.direction());
        const double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        const double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        const bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;
        if (cannot_refract ||
            reflectance(cos_theta, refraction_ratio) > random_double(rng)) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered = ray(rec.p, direction);
        return true;
    }

  private:
    static double reflectance(double cosine, double refraction_index) {
        double r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }

    double index_of_refraction_;
};

class sphere {
  public:
    sphere(const point3& center, double radius, std::shared_ptr<material> mat)
        : center_(center), radius_(radius), mat_(mat) {}

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
        const vec3 oc = r.origin() - center_;
        const double a = r.direction().length_squared();
        const double half_b = dot(oc, r.direction());
        const double c = oc.length_squared() - radius_ * radius_;
        const double discriminant = half_b * half_b - a * c;

        if (discriminant < 0) {
            return false;
        }

        const double sqrtd = std::sqrt(discriminant);
        double root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root) {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root) {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        const vec3 outward_normal = (rec.p - center_) / radius_;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat_;
        return true;
    }

  private:
    point3 center_;
    double radius_;
    std::shared_ptr<material> mat_;
};

using scene = std::vector<sphere>;

bool hit_scene(const scene& world, const ray& r, double t_min, double t_max,
               hit_record& rec) {
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;

    for (const sphere& object : world) {
        if (object.hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

color scene_ray_color(const ray& r, const scene& world, int depth,
                      std::mt19937& rng) {
    if (depth <= 0) {
        return color(0, 0, 0);
    }

    hit_record rec;
    if (hit_scene(world, r, 0.001, std::numeric_limits<double>::infinity(),
                  rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered, rng)) {
            return attenuation * scene_ray_color(scattered, world, depth - 1, rng);
        }

        return color(0, 0, 0);
    }

    const vec3 unit_direction = unit_vector(r.direction());
    const double a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.58, 0.75, 1.0);
}

class camera {
  public:
    camera(point3 lookfrom, point3 lookat, vec3 vup, double vertical_fov,
           double aspect) {
        const double theta = vertical_fov * 3.1415926535897932385 / 180.0;
        const double h = std::tan(theta / 2);
        const double viewport_height = 2.0 * h;
        const double viewport_width = aspect * viewport_height;

        const vec3 w = unit_vector(lookfrom - lookat);
        const vec3 u = unit_vector(cross(vup, w));
        const vec3 v = cross(w, u);

        origin_ = lookfrom;
        horizontal_ = viewport_width * u;
        vertical_ = viewport_height * v;
        lower_left_corner_ =
            origin_ - horizontal_ / 2 - vertical_ / 2 - w;
    }

    ray get_ray(double s, double t) const {
        return ray(origin_,
                   lower_left_corner_ + s * horizontal_ + t * vertical_ -
                       origin_);
    }

  private:
    point3 origin_;
    point3 lower_left_corner_;
    vec3 horizontal_;
    vec3 vertical_;
};

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

bool render_antialias_sphere(const std::filesystem::path& output_path) {
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

    std::mt19937 rng(1337);

    for (int y = image_height - 1; y >= 0; --y) {
        for (int x = 0; x < image_width; ++x) {
            color pixel_color(0, 0, 0);

            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                const double u =
                    (static_cast<double>(x) + random_double(rng)) /
                    (image_width - 1);
                const double v =
                    (static_cast<double>(y) + random_double(rng)) /
                    (image_height - 1);
                const ray r(camera_origin,
                            lower_left_corner + u * horizontal + v * vertical -
                                camera_origin);

                pixel_color += ray_color(r);
            }

            write_color(out, pixel_color, samples_per_pixel);
        }
    }

    std::cout << "Rendered " << output_path << " (" << image_width << "x"
              << image_height << ", " << samples_per_pixel
              << " samples per pixel)\n";
    return true;
}

bool render_minimal_raytracer(const std::filesystem::path& output_path) {
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    const auto ground_mat = std::make_shared<lambertian>(color(0.78, 0.74, 0.64));
    const auto center_mat = std::make_shared<lambertian>(color(0.18, 0.42, 0.72));
    const auto left_mat = std::make_shared<metal>(color(0.86, 0.78, 0.62), 0.08);
    const auto right_mat = std::make_shared<metal>(color(0.78, 0.84, 0.92), 0.22);
    const auto accent_mat = std::make_shared<lambertian>(color(0.86, 0.24, 0.48));

    const scene world = {
        sphere(point3(0, -100.5, -1), 100.0, ground_mat),
        sphere(point3(0, 0.15, -1.15), 0.5, center_mat),
        sphere(point3(-0.95, 0.08, -1.35), 0.42, left_mat),
        sphere(point3(0.95, 0.0, -1.05), 0.38, right_mat),
        sphere(point3(0.35, -0.22, -0.45), 0.18, accent_mat),
    };

    const camera cam(point3(2.1, 1.1, 1.4), point3(0, 0, -1.05),
                     vec3(0, 1, 0), 38.0, aspect_ratio);

    std::ofstream out;
    if (!open_ppm(output_path, image_width, image_height, out)) {
        return false;
    }

    std::mt19937 rng(2026);

    for (int y = image_height - 1; y >= 0; --y) {
        for (int x = 0; x < image_width; ++x) {
            color pixel_color(0, 0, 0);

            for (int sample = 0; sample < minimal_scene_samples_per_pixel;
                 ++sample) {
                const double u =
                    (static_cast<double>(x) + random_double(rng)) /
                    (image_width - 1);
                const double v =
                    (static_cast<double>(y) + random_double(rng)) /
                    (image_height - 1);
                pixel_color += scene_ray_color(cam.get_ray(u, v), world,
                                               max_ray_depth, rng);
            }

            write_color(out, pixel_color, minimal_scene_samples_per_pixel, true);
        }
    }

    std::cout << "Rendered " << output_path << " (" << image_width << "x"
              << image_height << ", " << minimal_scene_samples_per_pixel
              << " samples per pixel)\n";
    return true;
}

void write_metadata(const std::filesystem::path& metadata_path,
                    const std::string& scene_name,
                    const std::filesystem::path& output_ppm,
                    const std::filesystem::path& output_png, int image_height,
                    int preset_samples_per_pixel, long long render_time_ms,
                    int sphere_count,
                    const std::vector<std::string>& materials_used,
                    const std::string& camera_description) {
    std::ofstream out(metadata_path);
    if (!out) {
        std::cerr << "Failed to open " << metadata_path << " for writing.\n";
        return;
    }

    out << "{\n";
    out << "  \"scene_name\": \"" << scene_name << "\",\n";
    out << "  \"output_ppm\": \"" << output_ppm.string() << "\",\n";
    out << "  \"output_png\": \"" << output_png.string() << "\",\n";
    out << "  \"image_width\": " << image_width << ",\n";
    out << "  \"image_height\": " << image_height << ",\n";
    out << "  \"samples_per_pixel\": " << preset_samples_per_pixel << ",\n";
    out << "  \"max_depth\": " << max_ray_depth << ",\n";
    out << "  \"render_time_ms\": " << render_time_ms << ",\n";
    out << "  \"sphere_count\": " << sphere_count << ",\n";
    out << "  \"materials_used\": [";
    for (std::size_t i = 0; i < materials_used.size(); ++i) {
        out << "\"" << materials_used[i] << "\"";
        if (i + 1 < materials_used.size()) {
            out << ", ";
        }
    }
    out << "],\n";
    out << "  \"camera_description\": \"" << camera_description << "\"\n";
    out << "}\n";
}

bool render_scene_preset(const std::string& scene_name, const scene& world,
                         const camera& cam,
                         const std::filesystem::path& output_ppm,
                         const std::filesystem::path& output_png,
                         const std::filesystem::path& metadata_path,
                         const std::vector<std::string>& materials_used,
                         const std::string& camera_description,
                         unsigned int seed) {
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    constexpr int preset_samples_per_pixel = 80;

    std::ofstream out;
    if (!open_ppm(output_ppm, image_width, image_height, out)) {
        return false;
    }

    std::mt19937 rng(seed);
    const auto start = std::chrono::steady_clock::now();

    for (int y = image_height - 1; y >= 0; --y) {
        for (int x = 0; x < image_width; ++x) {
            color pixel_color(0, 0, 0);

            for (int sample = 0; sample < preset_samples_per_pixel; ++sample) {
                const double u =
                    (static_cast<double>(x) + random_double(rng)) /
                    (image_width - 1);
                const double v =
                    (static_cast<double>(y) + random_double(rng)) /
                    (image_height - 1);
                pixel_color += scene_ray_color(cam.get_ray(u, v), world,
                                               max_ray_depth, rng);
            }

            write_color(out, pixel_color, preset_samples_per_pixel, true);
        }
    }

    const auto end = std::chrono::steady_clock::now();
    const auto render_time_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();

    std::cout << "Rendered " << output_ppm << " (" << image_width << "x"
              << image_height << ", " << preset_samples_per_pixel
              << " samples per pixel)\n";

    write_metadata(metadata_path, scene_name, output_ppm, output_png,
                   image_height, preset_samples_per_pixel, render_time_ms,
                   static_cast<int>(world.size()), materials_used,
                   camera_description);
    return true;
}

bool render_glass_orbs(const std::filesystem::path& output_dir) {
    const auto ground = std::make_shared<lambertian>(color(0.74, 0.80, 0.84));
    const auto glass = std::make_shared<dielectric>(1.5);
    const auto pale_blue = std::make_shared<lambertian>(color(0.34, 0.58, 0.86));
    const auto chrome = std::make_shared<metal>(color(0.82, 0.90, 0.96), 0.02);
    const auto teal = std::make_shared<metal>(color(0.42, 0.88, 0.90), 0.12);

    const scene world = {
        sphere(point3(0, -100.55, -1.1), 100.0, ground),
        sphere(point3(0.0, 0.1, -1.15), 0.52, glass),
        sphere(point3(-0.88, -0.02, -1.28), 0.36, chrome),
        sphere(point3(0.82, -0.08, -1.0), 0.31, teal),
        sphere(point3(0.16, -0.27, -0.55), 0.16, pale_blue),
    };

    const camera cam(point3(2.25, 1.0, 1.45), point3(0.0, -0.05, -1.05),
                     vec3(0, 1, 0), 36.0, aspect_ratio);

    return render_scene_preset(
        "glass_orbs", world, cam, output_dir / "glass_orbs.ppm",
        output_dir / "glass_orbs.png", output_dir / "glass_orbs.json",
        {"lambertian", "metal", "dielectric"},
        "Cool-toned camera looking slightly downward at refractive and metallic "
        "orbs.",
        3101);
}

bool render_xr_lens_demo(const std::filesystem::path& output_dir) {
    const auto ground = std::make_shared<lambertian>(color(0.70, 0.73, 0.78));
    const auto lens = std::make_shared<dielectric>(1.45);
    const auto display_blue =
        std::make_shared<lambertian>(color(0.16, 0.36, 0.92));
    const auto magenta = std::make_shared<metal>(color(0.92, 0.22, 0.74), 0.18);
    const auto cyan = std::make_shared<metal>(color(0.30, 0.95, 0.94), 0.08);
    const auto graphite = std::make_shared<metal>(color(0.45, 0.47, 0.52), 0.25);

    const scene world = {
        sphere(point3(0, -100.5, -1.05), 100.0, ground),
        sphere(point3(0.0, 0.08, -1.1), 0.55, lens),
        sphere(point3(-0.72, -0.1, -0.72), 0.18, display_blue),
        sphere(point3(0.72, -0.12, -0.76), 0.18, magenta),
        sphere(point3(-1.05, 0.04, -1.35), 0.28, cyan),
        sphere(point3(1.08, 0.02, -1.45), 0.30, graphite),
        sphere(point3(0.0, -0.31, -0.48), 0.12, display_blue),
    };

    const camera cam(point3(2.0, 0.85, 1.65), point3(0.0, -0.03, -1.0),
                     vec3(0, 1, 0), 34.0, aspect_ratio);

    return render_scene_preset(
        "xr_lens_demo", world, cam, output_dir / "xr_lens_demo.ppm",
        output_dir / "xr_lens_demo.png", output_dir / "xr_lens_demo.json",
        {"lambertian", "metal", "dielectric"},
        "XR-inspired composition with a central refractive lens and small "
        "colored display-orb accents.",
        3102);
}

bool render_warm_studio_spheres(const std::filesystem::path& output_dir) {
    const auto ground = std::make_shared<lambertian>(color(0.82, 0.75, 0.66));
    const auto clay = std::make_shared<lambertian>(color(0.86, 0.40, 0.24));
    const auto gold = std::make_shared<metal>(color(0.94, 0.76, 0.42), 0.09);
    const auto pearl = std::make_shared<metal>(color(0.92, 0.86, 0.78), 0.28);
    const auto rose = std::make_shared<lambertian>(color(0.78, 0.22, 0.32));

    const scene world = {
        sphere(point3(0, -100.52, -1.1), 100.0, ground),
        sphere(point3(-0.25, 0.08, -1.0), 0.48, clay),
        sphere(point3(0.72, -0.02, -1.18), 0.40, gold),
        sphere(point3(-0.92, -0.04, -1.35), 0.34, pearl),
        sphere(point3(0.14, -0.29, -0.48), 0.17, rose),
    };

    const camera cam(point3(2.05, 0.95, 1.35), point3(-0.05, -0.03, -1.05),
                     vec3(0, 1, 0), 37.0, aspect_ratio);

    return render_scene_preset(
        "warm_studio_spheres", world, cam,
        output_dir / "warm_studio_spheres.ppm",
        output_dir / "warm_studio_spheres.png",
        output_dir / "warm_studio_spheres.json", {"lambertian", "metal"},
        "Warm studio-style camera looking across a compact sphere arrangement.",
        3103);
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

    if (!render_antialias_sphere(output_dir / "antialias_sphere.ppm")) {
        return 1;
    }

    if (!render_minimal_raytracer(output_dir / "minimal_raytracer.ppm")) {
        return 1;
    }

    if (!render_glass_orbs(output_dir)) {
        return 1;
    }

    if (!render_xr_lens_demo(output_dir)) {
        return 1;
    }

    if (!render_warm_studio_spheres(output_dir)) {
        return 1;
    }

    return 0;
}
