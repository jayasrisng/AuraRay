# AuraRay Foveation Toolkit

AuraRay Foveation Toolkit is an XR-focused Unity package for simulating gaze-aware foveated rendering. It visualizes a movable high-quality region and estimated sample budgets without requiring eye-tracking hardware or a native rendering plugin.

## Quick start

This repository embeds the package at `unity/AuraRayViewer/Packages/com.auraray.foveation`.

1. Open `unity/AuraRayViewer` with Unity 6000.3.17f1 or newer.
2. Open **Window > Package Management > Package Manager**.
3. Select **AuraRay Foveation Toolkit** and import **Interactive Foveation Demo**.
4. Open the imported `AuraRayViewer.unity` scene and enter Play Mode.

To use the toolkit in another local Unity project, choose **Add package from disk** in Package Manager and select this package's `package.json` file.

### Clean-install verification

Version `0.1.0` was installed from its local `package.json` into a fresh Unity 6000.3.17f1 project. Unity resolved the declared UGUI dependency, compiled the runtime and editor assemblies, imported the sample through Package Manager, and opened the sample scene with zero missing scripts.

## Components

- `GazeTargetController`: provides a clamped normalized gaze point and moves a visible target using keyboard or mouse input.
- `FoveationOverlayController`: draws configurable foveal, transition, and peripheral quality regions and estimates their sample budget.
- `AuraRayModeController`: coordinates gaze, overlay, optional statistics, and optional mode buttons.
- `AuraRayStatsPanel`: presents gaze coordinates and estimated sample usage through optional UGUI text fields.
- `FoveationMode`: defines the four supported visualization modes.

## Inspector settings

- **Mode Controller:** gaze provider, overlay, optional stats panel, optional mode buttons, and starting mode.
- **Gaze Target:** visible target transform, scene camera, movement speed, and normalized starting gaze.
- **Foveation Regions:** foveal radius and transition radius. Validation keeps the foveal radius smaller.
- **Sample Budget:** center, middle, and periphery sample counts, each clamped to at least one.
- **Appearance:** global overlay opacity, ring visibility, deterministic noise strength, and peripheral dimming strength.
- **Stats Panel:** optional title and body text references.

Defaults match the demo: `64/24/8` samples, `0.18/0.36` radii, visible rings, full opacity, and `0.42` gaze movement speed.

## Controls

- Move gaze: `WASD` or arrow keys
- Place gaze: left mouse click
- Change mode: keys `1` through `4`, or the on-screen buttons

## Sample scene

The **Interactive Foveation Demo** includes all four modes, a movable gaze target, and a live stats panel. The source sample is under `Samples~/InteractiveFoveationDemo`; import it through Package Manager before opening it.

## Current limitations

- Foveation is a visual simulation, not a reduction in Unity rendering workload.
- The gaze point is simulated rather than supplied by eye-tracking hardware.
- Sample budget numbers are explanatory estimates based on the AuraRay reference resolution.
- The toolkit currently uses Unity's built-in input APIs and UGUI.

## Roadmap

- Publish the `v0.1.0` tag with final demo media.
- Add focused EditMode tests for gaze clamping and sample-budget validation.
- Consider a replaceable gaze-provider interface after a concrete second input source exists.
- Later phases may add optional OpenXR eye-gaze input and native C++ renderer integration.

## License

AuraRay Foveation Toolkit is available under the [MIT License](https://github.com/jayasrisng/AuraRay/blob/main/LICENSE).
