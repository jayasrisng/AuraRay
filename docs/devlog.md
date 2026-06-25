# AuraRay Devlog

## Milestone 1: First Windowless Render

### What we built
- A minimal windowless C++ app that writes `renders/first_image.ppm`.
- A small `Makefile` so the project can build with the compiler already available on this Mac.

### Why we built it
- The first milestone should prove the core rendering pipeline before adding ray tracing, Unity, XR framing, or build-system complexity.
- PPM keeps the first output dependency-free and easy to inspect.

### Challenges
- CMake is not currently available on PATH, so this milestone uses `make` plus `c++` directly.
- The workspace was not a git repository yet, so git setup is part of this first checkpoint.

### What changed
- Added `.gitignore`.
- Added `Makefile`.
- Added `src/main.cpp`.
- Added this devlog.

### Next milestone
- Add the first actual ray tracing primitive: a camera, rays, and one visible sphere.

## Milestone 2: First Sphere With Camera Rays

### What we built
- A tiny ray tracer that sends one camera ray through each pixel.
- Minimal `vec3`, `ray`, sphere hit-test, and camera viewport logic.
- A new render output: `renders/first_sphere.ppm`.
- A PNG export for the new sphere render: `renders/first_sphere.png`.

### Why we built it
- This is the first real ray tracing concept in AuraRay: pixels are computed by tracing rays into a scene.
- Surface normals are rendered as color because they make sphere geometry visible without adding materials, lights, shadows, or reflections.
- Keeping the first gradient render preserves the milestone 1 proof while the sphere render becomes the milestone 2 proof.

### Challenges
- The goal was to add ray tracing structure without expanding into a full engine too early.
- The render still uses simple PPM output so the C++ code stays dependency-free.
- PNG remains an export convenience handled by macOS `sips`.

### What changed
- Updated `src/main.cpp` with `vec3`, `ray`, sphere intersection, camera rays, and normal-color rendering.
- Updated `Makefile` so `make png` exports both milestone images.
- Updated `README.md` with basic build/export commands.
- Added `renders/first_sphere.ppm` and `renders/first_sphere.png`.

### Next milestone
- Add simple materials and lighting only after the one-sphere ray pipeline is committed and understood.

## Polish: PNG Export for Portfolio Use

### What we built
- A `make png` / `make export` target that converts `renders/first_image.ppm` to `renders/first_image.png`.

### Why we built it
- PPM stays as the renderer's raw output because it is simple, transparent, and dependency-free while learning rendering fundamentals.
- PNG is useful for GitHub, README images, portfolio screenshots, and social sharing because browsers and preview tools support it directly.

### Challenges
- We avoided image-writing libraries and kept conversion outside the renderer.
- The available lightweight macOS tool is `sips`, so the export flow uses that instead of adding ImageMagick or a C++ PNG dependency.

### What changed
- Updated `Makefile` with `png` and `export` targets.
- Generated a PNG version of the first render.

### Next milestone
- Add the first actual ray tracing primitive: a camera, rays, and one visible sphere.
