# AuraRay Devlog

## Milestone 1: First Windowless Render

### Built
- Created a minimal C++ application that writes `renders/first_image.ppm`.
- Added a lightweight `Makefile` for local builds.
- Initialized the project structure and `.gitignore`.

### Challenges
- Used a Makefile instead of CMake for the initial setup.
- Kept the renderer dependency-free by writing directly to the PPM format.

### Next
- Implement camera rays and render the first sphere.

## Milestone 2: First Sphere With Camera Rays

### Built
- Implemented `vec3`, `ray`, camera, and sphere intersection.
- Added normal-based shading.
- Generated `first_sphere.ppm` and PNG export.
- Updated the build scripts and README.

### Next
- Add anti-aliasing.

## Milestone 3: Anti-Aliased Rendering

### Built
- Added configurable multi-sample anti-aliasing.
- Implemented jittered ray sampling with a fixed random seed.
- Generated anti-aliased PPM and PNG outputs.

### Challenges
- Used deterministic sampling to keep render outputs reproducible.

### Next
- Add materials and recursive ray tracing.

## Milestone 4: Minimal Ray Tracer

### Built
- Added Lambertian and metal materials.
- Implemented recursive ray scattering.
- Added a ground sphere and multiple scene objects.
- Created `minimal_raytracer` render output.

### Challenges
- Limited recursion depth to keep rendering stable.

### Next
- Build reusable scene presets.

## Milestone 5: Scene Presets

### Built
- Added three predefined scenes:
  - `glass_orbs`
  - `xr_lens_demo`
  - `warm_studio_spheres`
- Added PNG exports and JSON metadata.
- Recorded render settings and timing information.

### Next
- Implement simulated foveated rendering.

## Milestone 6: Simulated Foveated Rendering

### Built
- Added Full Quality, Low Quality, and Gaze-Aware rendering modes.
- Implemented sample allocation based on a simulated gaze position.
- Added overlay visualization and render metadata.
- Generated comparison images for all render modes.

### Challenges
- Balanced image quality while keeping the sampling logic simple.

### Next
- Visualize the renderer inside Unity.

## Milestone 7: Unity Viewer

### Built
- Created a Unity project for viewing renderer outputs.
- Displayed rendered images alongside render metadata.
- Added controls for switching between rendering modes.
- Implemented gaze overlay visualization.

### Challenges
- Organized Unity assets while excluding generated project files.

### Next
- Replace the static viewer with an interactive simulator.

## Milestone 8: Interactive Foveation Simulator

### Built
- Replaced the image viewer with an interactive Unity scene.
- Added a movable gaze target controlled by keyboard or mouse.
- Implemented live foveation visualization.
- Added four visualization modes:
  - Full Quality
  - Low Quality
  - Gaze-Aware
  - Overlay
- Added a runtime statistics panel.

### Design Decision
The Unity project evolved from a static viewer into an interactive simulator while keeping the C++ renderer as the offline reference implementation.

### Next
- Package the simulator for reuse.

## Milestone 9: Unity Package

### Built
- Converted the simulator into an embedded Unity package.
- Split runtime and editor assemblies.
- Added an importable sample scene.
- Documented installation and package usage.

### Challenges
- Preserved Unity GUIDs while reorganizing package assets.

### Next
- Improve component configurability.

## Milestone 10: Reusable Components

### Built
- Added Inspector-friendly configuration for all runtime components.
- Exposed gaze radius, sample counts, overlay settings, and movement controls.
- Added runtime validation and improved API documentation.

### Next
- Verify installation in a clean Unity project.

## Milestone 11: Package Verification

### Built
- Tested package installation in a clean Unity project.
- Verified sample import through the Unity Package Manager.
- Confirmed runtime components, scene loading, and package dependencies.

### Next
- Improve native build portability.

## Milestone 12: CMake Support

### Built
- Added a root `CMakeLists.txt`.
- Added Makefile helpers for configure, build, and run.
- Updated ignore rules and documentation.

### Next
- Prepare the first public release.

## Milestone 13: Release Preparation

### Built
- Reorganized the README.
- Added `CHANGELOG.md`.
- Added architecture documentation.
- Completed Unity package metadata.

### Next
- Capture release media.

## Milestone 14: Demo Media

### Built
- Added automated Unity capture tooling.
- Generated release screenshots.
- Created an animated simulator GIF.
- Generated a labeled comparison image from renderer outputs.
- Documented the media generation workflow.

### Challenges
- Built the media pipeline using native macOS tools without external image-processing dependencies.

### Next
- Finalize the v0.1.0 release.

## Miscellaneous Improvements

### PNG Export

#### Built
- Added `make png` and `make export` commands.
- Automated PPM to PNG conversion for renderer outputs.

#### Next
- Continue improving renderer features while keeping export workflows lightweight.
