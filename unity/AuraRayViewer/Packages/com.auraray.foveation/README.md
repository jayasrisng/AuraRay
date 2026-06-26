# AuraRay Foveation Toolkit

AuraRay Foveation Toolkit is an XR-focused Unity package for simulating gaze-aware foveated rendering. It visualizes a movable high-quality region and estimated sample budgets without requiring eye-tracking hardware or a native rendering plugin.

## Install path

This repository embeds the package at:

`unity/AuraRayViewer/Packages/com.auraray.foveation`

To use it in another local Unity project, open Package Manager, choose **Add package from disk**, and select this package's `package.json` file.

## Components

- `GazeTargetController`: moves a normalized gaze point with keyboard or mouse input.
- `FoveationOverlayController`: draws the foveal center, transition ring, and peripheral quality visualization.
- `AuraRayModeController`: switches Full Quality, Low Quality, Gaze-Aware, and Overlay modes.
- `AuraRayStatsPanel`: presents gaze coordinates and estimated sample usage.
- `FoveationMode`: defines the four supported visualization modes.

## Sample scene

In Package Manager, select **AuraRay Foveation Toolkit** and import **Interactive Foveation Demo**. Open `AuraRayViewer.unity` from the imported sample and enter Play Mode.

## Controls

- Move gaze: `WASD` or arrow keys
- Place gaze: left mouse click
- Change mode: keys `1` through `4`, or the on-screen buttons

## Current limitations

- Foveation is a visual simulation, not a reduction in Unity rendering workload.
- The gaze point is simulated rather than supplied by eye-tracking hardware.
- Sample budget numbers are explanatory estimates based on the AuraRay reference resolution.
- The toolkit currently uses Unity's built-in input APIs and UGUI.

## Future direction

The next step is an inspector-friendly component API. Later phases may connect real OpenXR eye-gaze input and a native C++ renderer plugin, while keeping those integrations optional.
