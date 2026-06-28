# Changelog

All notable changes to AuraRay are documented in this file.

## [0.1.0] - 2026-06-26

### Added

- C++17 offline ray tracer with camera rays, anti-aliasing, sphere geometry, Lambertian, metal, and dielectric materials.
- Deterministic scene presets with PPM output, PNG export, and JSON render metadata.
- Full-quality, low-quality, gaze-aware, and overlay renders for comparing simulated foveated sampling.
- Interactive Unity foveation simulator with movable gaze, four visualization modes, configurable sample regions, and live statistics.
- Reusable `com.auraray.foveation` Unity package with runtime/editor assemblies and an importable demo sample.
- Fresh Unity project verification covering local package installation, declared dependencies, sample import, scene loading, and missing-script checks.
- Makefile and CMake build workflows for the C++ renderer.
- Modular C++ renderer library with separated core, geometry, material, rendering, scene, foveation, and IO components.
- Deterministic render validation through CTest and committed SHA-256 expectations.
