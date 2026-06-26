# AuraRay Architecture

AuraRay separates offline rendering research from interactive XR visualization. The C++ renderer and Unity package exchange artifacts rather than sharing a native runtime.

```text
+-------------------------+
| C++ Offline Renderer    |
| src/main.cpp            |
+------------+------------+
             | writes
             v
+-------------------------+
| Render Artifacts        |
| PPM + PNG + JSON        |
| renders/                |
+------------+------------+
             | informs and provides reference output for
             v
+-------------------------+
| Unity Package           |
| com.auraray.foveation   |
+------------+------------+
             | powers
             v
+-------------------------+
| Interactive Simulator   |
| movable simulated gaze  |
| quality-region overlay  |
+-------------------------+
```

## C++ renderer

The renderer produces deterministic reference images and metadata. Its foveated modes vary sample counts by distance from a simulated gaze point. It is built independently with Make or CMake.

## Render artifacts

PPM files are the renderer's dependency-free raw output. PNG files are presentation exports, and JSON files record scene, camera, sampling, ray-budget, and timing metadata.

## Unity package

The embedded Unity package provides reusable components for gaze movement, foveation overlays, mode switching, and statistics. It does not execute the C++ renderer.

## Interactive simulator

The included sample uses Unity primitives and UGUI to demonstrate how a high-quality region could follow gaze. This is a visual simulation, not hardware variable-rate shading or eye tracking.
