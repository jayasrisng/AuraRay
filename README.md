# AuraRay
A minimal C++ ray tracer exploring gaze-aware rendering for XR.

## Build

```bash
make run
```

## Export portfolio-friendly images

```bash
make png
```

Current outputs:
- `renders/first_image.ppm` / `renders/first_image.png`
- `renders/first_sphere.ppm` / `renders/first_sphere.png`
- `renders/antialias_sphere.ppm` / `renders/antialias_sphere.png`
- `renders/minimal_raytracer.ppm` / `renders/minimal_raytracer.png`
- `renders/glass_orbs.ppm` / `renders/glass_orbs.png`
- `renders/xr_lens_demo.ppm` / `renders/xr_lens_demo.png`
- `renders/warm_studio_spheres.ppm` / `renders/warm_studio_spheres.png`

## Milestone 1: Minimal Ray Tracer

AuraRay now renders a small Ray Tracing in One Weekend-style scene with camera rays, anti-aliasing, diffuse materials, metal materials, and multiple spheres over a ground sphere.

![Minimal ray tracer scene](renders/minimal_raytracer.png)

## Scene Presets

These are intentionally composed sphere-only scenes that make AuraRay visually distinct from the tutorial baseline while keeping the renderer small.

| glass_orbs | xr_lens_demo | warm_studio_spheres |
| --- | --- | --- |
| ![glass_orbs](renders/glass_orbs.png) | ![xr_lens_demo](renders/xr_lens_demo.png) | ![warm_studio_spheres](renders/warm_studio_spheres.png) |

## Gallery

| Milestone | Output |
| --- | --- |
| First image pipeline | `renders/first_image.png` |
| First camera-ray sphere | `renders/first_sphere.png` |
| Anti-aliased sphere | `renders/antialias_sphere.png` |
| Minimal ray tracer scene | `renders/minimal_raytracer.png` |
| Pretty scene preset: glass_orbs | `renders/glass_orbs.png` |
| Pretty scene preset: xr_lens_demo | `renders/xr_lens_demo.png` |
| Pretty scene preset: warm_studio_spheres | `renders/warm_studio_spheres.png` |
