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
