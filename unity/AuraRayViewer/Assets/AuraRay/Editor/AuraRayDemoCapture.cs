using System;
using System.IO;
using AuraRay;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;

namespace AuraRayEditor
{
    public static class AuraRayDemoCapture
    {
        private const string ScenePath = "Assets/AuraRay/Scenes/AuraRayViewer.unity";
        private const int FrameCount = 60;
        private const int FrameWidth = 960;
        private const int FrameHeight = 600;

        public static void Capture()
        {
            EditorSceneManager.OpenScene(ScenePath, OpenSceneMode.Single);

            Camera camera = Camera.main;
            AuraRayModeController mode = UnityEngine.Object.FindFirstObjectByType<AuraRayModeController>();
            GazeTargetController gaze = UnityEngine.Object.FindFirstObjectByType<GazeTargetController>();
            if (camera == null || mode == null || gaze == null)
            {
                Fail("The demo scene is missing its camera, mode controller, or gaze controller.");
                return;
            }

            string repositoryRoot = Path.GetFullPath(Path.Combine(Application.dataPath, "../../.."));
            string mediaDirectory = Path.Combine(repositoryRoot, "docs/media");
            string frameDirectory = Environment.GetEnvironmentVariable("AURARAY_DEMO_FRAME_DIR");
            if (string.IsNullOrWhiteSpace(frameDirectory))
            {
                frameDirectory = Path.Combine(Path.GetTempPath(), "AuraRayDemoFrames");
            }
            Directory.CreateDirectory(mediaDirectory);
            RecreateFrameDirectory(frameDirectory);

            gaze.SetNormalizedGaze(new Vector2(0.64f, 0.58f));
            mode.ApplyMode(FoveationMode.GazeAware);
            CaptureFrame(camera, Path.Combine(mediaDirectory, "auraray_hero.png"), 1440, 900);

            for (int frame = 0; frame < FrameCount; frame++)
            {
                int modeIndex = Mathf.Min(frame / 15, 3);
                float phase = (frame % 15) / 14.0f;
                float direction = modeIndex % 2 == 0 ? phase : 1.0f - phase;
                float x = Mathf.Lerp(0.22f, 0.78f, direction);
                float y = 0.52f + 0.13f * Mathf.Sin(frame / (float)(FrameCount - 1) * Mathf.PI * 2.0f);

                gaze.SetNormalizedGaze(new Vector2(x, y));
                mode.ApplyModeIndex(modeIndex);
                CaptureFrame(camera, Path.Combine(frameDirectory, $"frame_{frame:000}.png"), FrameWidth, FrameHeight);
            }

            Debug.Log($"AURARAY_DEMO_CAPTURE_OK hero={mediaDirectory} frames={frameDirectory} count={FrameCount}");
        }

        private static void CaptureFrame(Camera camera, string outputPath, int width, int height)
        {
            RenderTexture renderTexture = new RenderTexture(width, height, 24);
            Texture2D image = new Texture2D(width, height, TextureFormat.RGB24, false);

            camera.targetTexture = renderTexture;
            RenderTexture.active = renderTexture;
            Canvas.ForceUpdateCanvases();
            camera.Render();
            image.ReadPixels(new Rect(0, 0, width, height), 0, 0);
            image.Apply();

            camera.targetTexture = null;
            RenderTexture.active = null;
            File.WriteAllBytes(outputPath, image.EncodeToPNG());

            UnityEngine.Object.DestroyImmediate(image);
            UnityEngine.Object.DestroyImmediate(renderTexture);
        }

        private static void RecreateFrameDirectory(string frameDirectory)
        {
            if (Directory.Exists(frameDirectory))
            {
                Directory.Delete(frameDirectory, true);
            }

            Directory.CreateDirectory(frameDirectory);
        }

        private static void Fail(string message)
        {
            Debug.LogError("AURARAY_DEMO_CAPTURE_FAILED " + message);
            EditorApplication.Exit(1);
        }
    }
}
