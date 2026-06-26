using System.IO;
using AuraRay;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace AuraRayEditor
{
    public static class AuraRaySceneBuilder
    {
        private const string ScenePath = "Assets/AuraRay/Scenes/AuraRayViewer.unity";

        public static void BuildScene()
        {
            EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);

            GameObject cameraObject = new GameObject("Main Camera", typeof(Camera));
            Camera camera = cameraObject.GetComponent<Camera>();
            camera.clearFlags = CameraClearFlags.SolidColor;
            camera.backgroundColor = new Color(0.015f, 0.018f, 0.028f);
            cameraObject.tag = "MainCamera";

            GameObject viewerObject = new GameObject("AuraRayViewer", typeof(AuraRayViewerController));
            AuraRayViewerController controller = viewerObject.GetComponent<AuraRayViewerController>();
            controller.BuildInterface();
            controller.ShowMode(2);

            Directory.CreateDirectory("Assets/AuraRay/Scenes");
            EditorSceneManager.SaveScene(EditorSceneManager.GetActiveScene(), ScenePath);
            EditorBuildSettings.scenes = new[]
            {
                new EditorBuildSettingsScene(ScenePath, true)
            };

            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
        }

        public static void CaptureScreenshot()
        {
            Scene scene = EditorSceneManager.OpenScene(ScenePath, OpenSceneMode.Single);
            AuraRayViewerController controller = Object.FindFirstObjectByType<AuraRayViewerController>();
            if (controller != null)
            {
                controller.ShowMode(2);
            }

            Camera camera = Camera.main;
            if (camera == null)
            {
                Debug.LogError("AuraRay screenshot failed: Main Camera was not found.");
                EditorApplication.Exit(1);
                return;
            }

            const int width = 1440;
            const int height = 900;
            RenderTexture renderTexture = new RenderTexture(width, height, 24);
            Texture2D screenshot = new Texture2D(width, height, TextureFormat.RGB24, false);

            camera.targetTexture = renderTexture;
            RenderTexture.active = renderTexture;
            camera.Render();
            screenshot.ReadPixels(new Rect(0, 0, width, height), 0, 0);
            screenshot.Apply();

            camera.targetTexture = null;
            RenderTexture.active = null;

            string outputPath = Path.GetFullPath(Path.Combine(Application.dataPath, "../../../docs/screenshots/unity_aura_viewer.png"));
            Directory.CreateDirectory(Path.GetDirectoryName(outputPath));
            File.WriteAllBytes(outputPath, screenshot.EncodeToPNG());

            Object.DestroyImmediate(screenshot);
            Object.DestroyImmediate(renderTexture);

            Debug.Log($"AuraRay screenshot saved to {outputPath}");
            EditorSceneManager.SaveScene(scene);
        }

        public static void SmokeTestModes()
        {
            EditorSceneManager.OpenScene(ScenePath, OpenSceneMode.Single);
            AuraRayViewerController controller = Object.FindFirstObjectByType<AuraRayViewerController>();
            if (controller == null)
            {
                Debug.LogError("AuraRay smoke test failed: viewer controller was not found.");
                EditorApplication.Exit(1);
                return;
            }

            string[] expectedLabels =
            {
                "Full Quality",
                "Low Quality",
                "Gaze-Aware",
                "Overlay / Heatmap"
            };

            for (int i = 0; i < expectedLabels.Length; i++)
            {
                controller.ShowMode(i);
                Canvas.ForceUpdateCanvases();

                if (controller.CurrentModeLabel != expectedLabels[i])
                {
                    Debug.LogError($"AuraRay smoke test failed: expected mode '{expectedLabels[i]}', got '{controller.CurrentModeLabel}'.");
                    EditorApplication.Exit(1);
                    return;
                }

                if (string.IsNullOrWhiteSpace(controller.CurrentMetadataText) ||
                    !controller.CurrentMetadataText.Contains("estimated rays"))
                {
                    Debug.LogError($"AuraRay smoke test failed: metadata did not update for mode '{expectedLabels[i]}'.");
                    EditorApplication.Exit(1);
                    return;
                }
            }

            Debug.Log("AuraRay smoke test passed: all viewer modes and metadata panels update.");
        }
    }
}
