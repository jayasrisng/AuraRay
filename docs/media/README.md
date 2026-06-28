# AuraRay Demo Media

The release media is generated from committed renderer outputs and the Unity simulator scene.

## Generate everything

On macOS with Unity 6000.3.17f1 installed at the default Unity Hub path:

```bash
make demo-media
```

This command:

1. Composes `foveated_comparison.png` from the four C++ foveated PNG outputs.
2. Opens the Unity demo scene in batchmode.
3. Captures `auraray_hero.png` and 60 deterministic animation frames.
4. Stores temporary frames under `/tmp/AuraRayDemoFrames`.
5. Encodes `auraray_demo.gif` with macOS Swift, AppKit, and ImageIO.

Override the editor path when needed:

```bash
make demo-media UNITY="/path/to/Unity"
```

## Generate only the comparison

```bash
make comparison-image
```

## Manual video capture

For a narrated or higher-frame-rate demo clip, open `unity/AuraRayViewer`, run `AuraRayViewer.unity`, and record the Game view while:

1. Moving gaze with `WASD`, arrow keys, or mouse clicks.
2. Switching through Full Quality, Low Quality, Gaze-Aware, and Overlay with keys `1` through `4`.
3. Ending in Gaze-Aware mode with the gaze point away from center so the moving quality region is obvious.
