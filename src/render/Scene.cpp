#include "render/Scene.h"

#include <memory>
#include <utility>

#include "geometry/Sphere.h"
#include "materials/Dielectric.h"
#include "materials/Lambertian.h"
#include "materials/Metal.h"
#include "render/Config.h"

namespace auraray::render {
namespace {

using core::Color;
using core::Point3;
using core::Vec3;
using geometry::Sphere;
using materials::Dielectric;
using materials::Lambertian;
using materials::Metal;

template <typename MaterialType>
void addSphere(geometry::HittableList& world, const Point3& center,
               double radius, const std::shared_ptr<MaterialType>& material) {
    world.add(std::make_shared<Sphere>(center, radius, material));
}

}  // namespace

ScenePreset makeMinimalScene() {
    const auto ground = std::make_shared<Lambertian>(Color(0.78, 0.74, 0.64));
    const auto center = std::make_shared<Lambertian>(Color(0.18, 0.42, 0.72));
    const auto left = std::make_shared<Metal>(Color(0.86, 0.78, 0.62), 0.08);
    const auto right = std::make_shared<Metal>(Color(0.78, 0.84, 0.92), 0.22);
    const auto accent = std::make_shared<Lambertian>(Color(0.86, 0.24, 0.48));

    geometry::HittableList world;
    addSphere(world, Point3(0, -100.5, -1), 100.0, ground);
    addSphere(world, Point3(0, 0.15, -1.15), 0.5, center);
    addSphere(world, Point3(-0.95, 0.08, -1.35), 0.42, left);
    addSphere(world, Point3(0.95, 0.0, -1.05), 0.38, right);
    addSphere(world, Point3(0.35, -0.22, -0.45), 0.18, accent);

    return ScenePreset{
        "minimal_raytracer",
        std::move(world),
        core::Camera(Point3(2.1, 1.1, 1.4), Point3(0, 0, -1.05),
                     Vec3(0, 1, 0), 38.0, kAspectRatio),
        {"lambertian", "metal"},
        "Compact material demonstration scene.",
        2026,
    };
}

ScenePreset makeGlassOrbsScene() {
    const auto ground = std::make_shared<Lambertian>(Color(0.74, 0.80, 0.84));
    const auto glass = std::make_shared<Dielectric>(1.5);
    const auto paleBlue = std::make_shared<Lambertian>(Color(0.34, 0.58, 0.86));
    const auto chrome = std::make_shared<Metal>(Color(0.82, 0.90, 0.96), 0.02);
    const auto teal = std::make_shared<Metal>(Color(0.42, 0.88, 0.90), 0.12);

    geometry::HittableList world;
    addSphere(world, Point3(0, -100.55, -1.1), 100.0, ground);
    addSphere(world, Point3(0.0, 0.1, -1.15), 0.52, glass);
    addSphere(world, Point3(-0.88, -0.02, -1.28), 0.36, chrome);
    addSphere(world, Point3(0.82, -0.08, -1.0), 0.31, teal);
    addSphere(world, Point3(0.16, -0.27, -0.55), 0.16, paleBlue);

    return ScenePreset{
        "glass_orbs",
        std::move(world),
        core::Camera(Point3(2.25, 1.0, 1.45), Point3(0.0, -0.05, -1.05),
                     Vec3(0, 1, 0), 36.0, kAspectRatio),
        {"lambertian", "metal", "dielectric"},
        "Cool-toned camera looking slightly downward at refractive and metallic orbs.",
        3101,
    };
}

ScenePreset makeXrLensDemoScene() {
    const auto ground = std::make_shared<Lambertian>(Color(0.70, 0.73, 0.78));
    const auto lens = std::make_shared<Dielectric>(1.45);
    const auto displayBlue =
        std::make_shared<Lambertian>(Color(0.16, 0.36, 0.92));
    const auto magenta = std::make_shared<Metal>(Color(0.92, 0.22, 0.74), 0.18);
    const auto cyan = std::make_shared<Metal>(Color(0.30, 0.95, 0.94), 0.08);
    const auto graphite = std::make_shared<Metal>(Color(0.45, 0.47, 0.52), 0.25);

    geometry::HittableList world;
    addSphere(world, Point3(0, -100.5, -1.05), 100.0, ground);
    addSphere(world, Point3(0.0, 0.08, -1.1), 0.55, lens);
    addSphere(world, Point3(-0.72, -0.1, -0.72), 0.18, displayBlue);
    addSphere(world, Point3(0.72, -0.12, -0.76), 0.18, magenta);
    addSphere(world, Point3(-1.05, 0.04, -1.35), 0.28, cyan);
    addSphere(world, Point3(1.08, 0.02, -1.45), 0.30, graphite);
    addSphere(world, Point3(0.0, -0.31, -0.48), 0.12, displayBlue);

    return ScenePreset{
        "xr_lens_demo",
        std::move(world),
        core::Camera(Point3(2.0, 0.85, 1.65), Point3(0.0, -0.03, -1.0),
                     Vec3(0, 1, 0), 34.0, kAspectRatio),
        {"lambertian", "metal", "dielectric"},
        "XR-inspired composition with a central refractive lens and small colored display-orb accents.",
        3102,
    };
}

ScenePreset makeWarmStudioSpheresScene() {
    const auto ground = std::make_shared<Lambertian>(Color(0.82, 0.75, 0.66));
    const auto clay = std::make_shared<Lambertian>(Color(0.86, 0.40, 0.24));
    const auto gold = std::make_shared<Metal>(Color(0.94, 0.76, 0.42), 0.09);
    const auto pearl = std::make_shared<Metal>(Color(0.92, 0.86, 0.78), 0.28);
    const auto rose = std::make_shared<Lambertian>(Color(0.78, 0.22, 0.32));

    geometry::HittableList world;
    addSphere(world, Point3(0, -100.52, -1.1), 100.0, ground);
    addSphere(world, Point3(-0.25, 0.08, -1.0), 0.48, clay);
    addSphere(world, Point3(0.72, -0.02, -1.18), 0.40, gold);
    addSphere(world, Point3(-0.92, -0.04, -1.35), 0.34, pearl);
    addSphere(world, Point3(0.14, -0.29, -0.48), 0.17, rose);

    return ScenePreset{
        "warm_studio_spheres",
        std::move(world),
        core::Camera(Point3(2.05, 0.95, 1.35), Point3(-0.05, -0.03, -1.05),
                     Vec3(0, 1, 0), 37.0, kAspectRatio),
        {"lambertian", "metal"},
        "Warm studio-style camera looking across a compact sphere arrangement.",
        3103,
    };
}

}  // namespace auraray::render
