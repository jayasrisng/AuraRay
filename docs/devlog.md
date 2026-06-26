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

## Milestone 3: Simulated Foveated Rendering

### What we built
- A foveated rendering comparison using the existing `xr_lens_demo` scene.
- Full-quality rendering with high samples across the whole image.
- Low-quality rendering with low samples across the whole image.
- Gaze-aware rendering with high samples near a simulated gaze point, medium samples in a transition ring, and low samples in the periphery.
- A simple overlay visualization showing the gaze point and foveal rings.
- JSON metadata for each foveated output with ray budget estimates and render timings.

### Why we built it
- AuraRay needs to feel like an XR rendering project, not just a generic ray tracing tutorial clone.
- Foveated rendering creates a clear XR story: spend render work where the user is looking and save work elsewhere.
- Using the same scene for all three modes makes the quality tradeoff easy to compare in the README and portfolio.

### Challenges
- The sampling logic needed to stay simple and readable instead of becoming a full adaptive renderer.
- The metadata has to communicate ray budget differences without pretending to be real hardware eye tracking.
- The overlay needed to be useful without spending a lot of time on visualization tooling.

### What changed
- Updated `src/main.cpp` with foveated sample selection, full/low/gaze render modes, overlay generation, and foveated metadata writing.
- Updated `Makefile` so `make png` exports all foveated PNGs.
- Updated `README.md` with a foveated rendering comparison gallery.
- Added PPM, PNG, and JSON outputs for full, low, gaze-aware, and overlay modes.

### Next milestone
- Build a Unity XR display viewer.

## Milestone 4: Unity Aura Viewer

### What we built
- A Unity project at `unity/AuraRayViewer`.
- An `AuraRayViewer` scene with a dark XR-inspired display surface.
- A large render panel that shows the foveated PNG outputs from the C++ renderer.
- Buttons and keyboard shortcuts for Full Quality, Low Quality, Gaze-Aware, and Overlay / Heatmap modes.
- A metadata panel that updates with render mode, estimated rays, render time, samples, gaze point, and scene name.
- A gaze marker and foveal ring overlay for the gaze-aware view.
- A screenshot artifact at `docs/screenshots/unity_aura_viewer.png`.

### Why we built it
- AuraRay needs to feel like an XR product/demo, not only a command-line renderer.
- Unity is a viewer here, which keeps the C++ renderer separate and avoids native plugin complexity.
- Showing images and metadata together makes the foveated rendering tradeoff easier to understand.

### Challenges
- The Unity project needed to avoid committing generated `Library`, `Logs`, and user-specific files.
- Unity's default project did not include UGUI, so the viewer explicitly adds Unity's `com.unity.ugui` package.
- The gaze overlay needed to be visible in screenshots and play mode, so it is generated as a transparent overlay texture.

### What changed
- Added the Unity project scaffold under `unity/AuraRayViewer`.
- Copied the foveated PNG and JSON outputs into Unity `Resources`.
- Added small Unity scripts for metadata parsing, viewer control, and gaze overlay rendering.
- Added an editor scene builder/smoke test helper.
- Updated `.gitignore`, `README.md`, and this devlog.

### Next milestone
- Package/polish the demo or add interactive gaze control.

## Pivot Milestone 4: Interactive Unity Foveation Simulator

### What we built
- A real Unity 3D simulator scene using built-in primitives only.
- A movable `EyeTarget` reticle controlled by `WASD`, arrow keys, or mouse click.
- A real-time foveation overlay that follows the gaze target.
- Four Unity modes: Full Quality, Low Quality, Gaze-Aware, and Overlay.
- A stats panel showing normalized gaze coordinates, sample tiers, estimated ray budget, and a short usage note.
- A screenshot artifact at `docs/screenshots/unity_interactive_foveation.png`.

### Why we pivoted from static viewer to interactive simulator
- The static PNG viewer proved the foveated renderer output, but it felt more like a gallery than an XR interaction demo.
- AuraRay's portfolio value improves when the Unity side demonstrates the core XR idea live: the high-quality region moves with gaze.
- The C++ renderer remains the offline/reference renderer, while Unity becomes the interaction layer.

### Challenges
- The simulator needed to feel interactive without adding XR SDKs, external assets, custom packages, or a native plugin.
- The foveation effect had to be readable in screenshots, so it uses a simple screen-space overlay with rings, dimming, and deterministic noise.
- Batchmode screenshots required a camera-space canvas so UI and overlay render into the captured image.

### What changed
- Added `GazeTargetController`, `FoveationOverlayController`, `AuraRayModeController`, `AuraRayStatsPanel`, and `FoveationMode`.
- Rebuilt the `AuraRayViewer` scene around a primitive 3D demo scene instead of a static image panel.
- Updated the editor scene builder with simulator construction, screenshot capture, and smoke-test checks.
- Updated `README.md` and this devlog.

### Next milestone
- Polish the Unity package into a reusable foveation demo component.

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
