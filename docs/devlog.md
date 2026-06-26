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

## Milestone 3: Anti-Aliased Sphere Edges

### What we built
- A new anti-aliased sphere render at `renders/antialias_sphere.ppm`.
- A PNG export at `renders/antialias_sphere.png`.
- Configurable `samples_per_pixel` near the top of `src/main.cpp`.
- Random jitter inside each pixel so each final pixel averages multiple nearby camera rays.

### Why we built it
- Anti-aliasing makes the sphere edge look smoother and more portfolio-ready without changing the scene itself.
- This teaches the next core ray tracing concept: one pixel can represent an average of multiple samples, not just one ray.
- We kept materials, reflections, and lighting out so the milestone stays focused on sampling.

### Challenges
- Random sampling can make committed images change every run, so the renderer uses a fixed RNG seed for reproducible output.
- The renderer still writes PPM as the raw output and uses `sips` only for PNG export.

### What changed
- Updated `src/main.cpp` with jittered multi-sample rendering for the sphere.
- Updated `Makefile` so `make png` exports the anti-aliased render.
- Updated `README.md` with the new output and a small gallery table.
- Added `renders/antialias_sphere.ppm` and `renders/antialias_sphere.png`.

### Next milestone
- Consider simple diffuse materials only after comparing the jagged and anti-aliased sphere renders.

## Milestone 1: Minimal Ray Tracer Scene

### What we built
- A small portfolio-ready ray traced scene at `renders/minimal_raytracer.ppm`.
- A PNG export at `renders/minimal_raytracer.png`.
- A minimal material system with diffuse/lambertian and metal materials.
- Multiple spheres plus a large ground sphere.
- A camera view aimed at a composed scene instead of a single centered debug sphere.
- Anti-aliasing and bounded recursive ray bounces for the final scene.

### Why we built it
- This proves the C++ renderer can do the core Ray Tracing in One Weekend ideas: rays, sphere intersections, anti-aliasing, materials, recursive scattering, and camera composition.
- Diffuse and metal materials give enough visual variety for a portfolio screenshot without adding glass/refraction complexity yet.
- Keeping the scene small makes the project fast to build and easy to reason about before gaze-aware rendering.

### Challenges
- The material system needed to stay minimal instead of becoming a full rendering engine.
- Recursive ray color needs a max depth so diffuse bounces terminate predictably.
- Random sampling uses a fixed seed so committed render outputs stay reproducible.

### What changed
- Updated `src/main.cpp` with hit records, diffuse materials, metal materials, scene intersection, recursive shading, and a camera class.
- Updated `Makefile` so `make png` exports the minimal ray tracer scene.
- Updated `README.md` with a “Milestone 1: Minimal Ray Tracer” section and the new PNG.
- Added `renders/minimal_raytracer.ppm` and `renders/minimal_raytracer.png`.

### Next milestone
- Add gaze-aware/foveated rendering.

## Milestone 2: Pretty Scene Presets

### What we built
- Three named scene preset renders:
  - `glass_orbs`
  - `xr_lens_demo`
  - `warm_studio_spheres`
- PNG exports for all three presets.
- JSON metadata files beside each preset render with render settings, timing, sphere count, materials, and camera notes.
- A compact dielectric/glass material for the cool-toned orb and XR lens scenes.

### Why we built it
- The renderer already worked technically; this milestone makes the project look intentional and portfolio-ready.
- Spheres are enough to create distinct moods when camera placement, color, material choices, and composition are deliberate.
- Metadata makes each render more credible by documenting how it was generated.

### Challenges
- The code needed to stay simple instead of turning into a full scene system.
- Glass/refraction had to remain small and stable, so it was limited to one compact dielectric material.
- Previous proof renders needed to remain intact while adding new gallery-ready outputs.

### What changed
- Updated `src/main.cpp` with three explicit scene preset functions.
- Added metadata writing with `std::chrono` render timing.
- Updated `Makefile` so `make png` exports all preset PNGs.
- Updated `README.md` with a Scene Presets gallery.
- Added PPM, PNG, and JSON files for all three presets.

### Next milestone
- Add simulated gaze-aware/foveated rendering.

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
