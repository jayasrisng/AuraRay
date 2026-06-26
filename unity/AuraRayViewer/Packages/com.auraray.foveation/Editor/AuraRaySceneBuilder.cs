using System.Collections.Generic;
using System.IO;
using AuraRay;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

namespace AuraRayEditor
{
    public static class AuraRaySceneBuilder
    {
        private const string ScenePath = "Assets/AuraRay/Scenes/AuraRayViewer.unity";

        public static void BuildScene()
        {
            EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);

            Camera camera = CreateCamera();
            CreateLighting();
            CreateStudioScene();

            GameObject gazeObject = CreateGazeTarget();
            GazeTargetController gaze = new GameObject("GazeTargetController", typeof(GazeTargetController)).GetComponent<GazeTargetController>();
            gaze.Configure(gazeObject.transform, camera);

            Canvas canvas = CreateCanvas(camera);
            Font font = Resources.GetBuiltinResource<Font>("LegacyRuntime.ttf");
            FoveationOverlayController overlay = CreateOverlay(canvas.transform);
            AuraRayStatsPanel stats = CreateStatsPanel(canvas.transform, font);
            List<Button> buttons = CreateModeButtons(canvas.transform, font);

            AuraRayModeController modeController = new GameObject("AuraRayModeController", typeof(AuraRayModeController)).GetComponent<AuraRayModeController>();
            modeController.Configure(gaze, overlay, stats, buttons);

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
            AuraRayModeController mode = Object.FindFirstObjectByType<AuraRayModeController>();
            if (mode != null)
            {
                mode.ApplyMode(FoveationMode.GazeAware);
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
            Canvas.ForceUpdateCanvases();
            screenshot.ReadPixels(new Rect(0, 0, width, height), 0, 0);
            screenshot.Apply();

            camera.targetTexture = null;
            RenderTexture.active = null;

            string outputPath = Path.GetFullPath(Path.Combine(Application.dataPath, "../../../docs/screenshots/unity_interactive_foveation.png"));
            Directory.CreateDirectory(Path.GetDirectoryName(outputPath));
            File.WriteAllBytes(outputPath, screenshot.EncodeToPNG());

            Object.DestroyImmediate(screenshot);
            Object.DestroyImmediate(renderTexture);

            Debug.Log($"AuraRay interactive screenshot saved to {outputPath}");
            EditorSceneManager.SaveScene(scene);
        }

        public static void SmokeTestModes()
        {
            EditorSceneManager.OpenScene(ScenePath, OpenSceneMode.Single);

            AuraRayModeController mode = Object.FindFirstObjectByType<AuraRayModeController>();
            GazeTargetController gaze = Object.FindFirstObjectByType<GazeTargetController>();
            FoveationOverlayController overlay = Object.FindFirstObjectByType<FoveationOverlayController>();
            AuraRayStatsPanel stats = Object.FindFirstObjectByType<AuraRayStatsPanel>();
            GameObject gazeTarget = GameObject.Find("EyeTarget");

            if (mode == null || gaze == null || overlay == null || stats == null || gazeTarget == null)
            {
                Debug.LogError("AuraRay interactive smoke test failed: a required simulator object was missing.");
                EditorApplication.Exit(1);
                return;
            }

            Vector3 before = gazeTarget.transform.position;
            gaze.SetNormalizedGaze(new Vector2(0.72f, 0.58f));
            Vector3 after = gazeTarget.transform.position;
            if ((after - before).sqrMagnitude < 0.01f)
            {
                Debug.LogError("AuraRay interactive smoke test failed: gaze target did not move.");
                EditorApplication.Exit(1);
                return;
            }

            for (int i = 0; i < 4; i++)
            {
                mode.ApplyModeIndex(i);
                Canvas.ForceUpdateCanvases();
                if (mode.CurrentMode != (FoveationMode)i)
                {
                    Debug.LogError($"AuraRay interactive smoke test failed: mode {i} did not apply.");
                    EditorApplication.Exit(1);
                    return;
                }
            }

            Debug.Log("AuraRay interactive smoke test passed: scene, gaze movement, overlay, stats, and modes are wired.");
        }

        private static Camera CreateCamera()
        {
            GameObject cameraObject = new GameObject("Main Camera", typeof(Camera));
            cameraObject.tag = "MainCamera";
            cameraObject.transform.position = new Vector3(0.0f, 1.55f, -6.2f);
            cameraObject.transform.rotation = Quaternion.Euler(10.0f, 0.0f, 0.0f);

            Camera camera = cameraObject.GetComponent<Camera>();
            camera.fieldOfView = 45.0f;
            camera.clearFlags = CameraClearFlags.SolidColor;
            camera.backgroundColor = new Color(0.015f, 0.018f, 0.028f);
            return camera;
        }

        private static void CreateLighting()
        {
            RenderSettings.ambientLight = new Color(0.22f, 0.24f, 0.30f);

            GameObject key = new GameObject("Key Light", typeof(Light));
            key.transform.rotation = Quaternion.Euler(45, -35, 0);
            Light keyLight = key.GetComponent<Light>();
            keyLight.type = LightType.Directional;
            keyLight.intensity = 2.4f;
            keyLight.color = new Color(0.86f, 0.94f, 1.0f);

            GameObject fill = new GameObject("Soft Fill", typeof(Light));
            fill.transform.position = new Vector3(-3.0f, 2.2f, -2.5f);
            Light fillLight = fill.GetComponent<Light>();
            fillLight.type = LightType.Point;
            fillLight.range = 6.0f;
            fillLight.intensity = 1.7f;
            fillLight.color = new Color(0.35f, 0.62f, 1.0f);
        }

        private static void CreateStudioScene()
        {
            Material floor = Material("Mat_Floor", new Color(0.12f, 0.14f, 0.18f), 0.15f, 0.25f);
            Material blue = Material("Mat_AuraBlue", new Color(0.16f, 0.38f, 0.95f), 0.25f, 0.45f);
            Material cyan = Material("Mat_CyanOrb", new Color(0.1f, 0.86f, 1.0f), 0.45f, 0.35f);
            Material magenta = Material("Mat_MagentaOrb", new Color(0.95f, 0.18f, 0.62f), 0.30f, 0.35f);
            Material chrome = Material("Mat_ChromeOrb", new Color(0.72f, 0.78f, 0.86f), 0.75f, 0.12f);

            GameObject floorObject = GameObject.CreatePrimitive(PrimitiveType.Cube);
            floorObject.name = "Studio Floor";
            floorObject.transform.position = new Vector3(0, -0.08f, 0.2f);
            floorObject.transform.localScale = new Vector3(7.0f, 0.12f, 5.6f);
            floorObject.GetComponent<Renderer>().sharedMaterial = floor;

            CreateSphere("Central Lens Orb", new Vector3(0, 1.05f, 0.65f), 0.78f, blue);
            CreateSphere("Chrome Reference Orb", new Vector3(1.55f, 0.78f, 0.25f), 0.52f, chrome);
            CreateSphere("Cyan Display Orb", new Vector3(-1.35f, 0.72f, 0.1f), 0.44f, cyan);
            CreateSphere("Magenta Accent Orb", new Vector3(0.72f, 0.36f, -0.05f), 0.34f, magenta);
            CreateSphere("Small Tracking Orb", new Vector3(-0.55f, 0.33f, -0.25f), 0.24f, blue);
        }

        private static GameObject CreateGazeTarget()
        {
            Material targetMaterial = Material("Mat_GazeTarget", new Color(1.0f, 0.18f, 0.55f), 0.2f, 0.2f);
            GameObject target = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            target.name = "EyeTarget";
            target.transform.localScale = Vector3.one * 0.13f;
            target.GetComponent<Renderer>().sharedMaterial = targetMaterial;
            return target;
        }

        private static void CreateSphere(string name, Vector3 position, float radius, Material material)
        {
            GameObject sphere = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            sphere.name = name;
            sphere.transform.position = position;
            sphere.transform.localScale = Vector3.one * (radius * 2.0f);
            sphere.GetComponent<Renderer>().sharedMaterial = material;
        }

        private static Material Material(string name, Color color, float metallic, float smoothness)
        {
            Material material = new Material(Shader.Find("Standard"));
            material.name = name;
            material.color = color;
            material.SetFloat("_Metallic", metallic);
            material.SetFloat("_Glossiness", smoothness);
            return material;
        }

        private static Canvas CreateCanvas(Camera camera)
        {
            GameObject canvasObject = new GameObject("AuraRay Simulator Canvas", typeof(Canvas), typeof(CanvasScaler), typeof(GraphicRaycaster));
            Canvas canvas = canvasObject.GetComponent<Canvas>();
            canvas.renderMode = UnityEngine.RenderMode.ScreenSpaceCamera;
            canvas.worldCamera = camera;
            canvas.planeDistance = 3.0f;

            CanvasScaler scaler = canvasObject.GetComponent<CanvasScaler>();
            scaler.uiScaleMode = CanvasScaler.ScaleMode.ScaleWithScreenSize;
            scaler.referenceResolution = new Vector2(1440, 900);
            scaler.matchWidthOrHeight = 0.5f;
            return canvas;
        }

        private static FoveationOverlayController CreateOverlay(Transform parent)
        {
            GameObject overlayObject = new GameObject("Realtime Foveation Overlay", typeof(RectTransform), typeof(RawImage), typeof(FoveationOverlayController));
            RectTransform rect = overlayObject.GetComponent<RectTransform>();
            rect.SetParent(parent, false);
            rect.anchorMin = Vector2.zero;
            rect.anchorMax = Vector2.one;
            rect.offsetMin = Vector2.zero;
            rect.offsetMax = Vector2.zero;
            return overlayObject.GetComponent<FoveationOverlayController>();
        }

        private static AuraRayStatsPanel CreateStatsPanel(Transform parent, Font font)
        {
            RectTransform panel = CreatePanel(parent, "Stats Panel", new Vector2(1, 0.5f), new Vector2(320, 390), new Vector2(-185, 0), new Color(0.035f, 0.043f, 0.065f, 0.92f));
            Text title = CreateText(panel, "Interactive Gaze-Aware", font, 22, FontStyle.Bold, new Vector2(0, 150), new Vector2(260, 42), TextAnchor.MiddleLeft, new Color(0.94f, 0.97f, 1.0f));
            Text body = CreateText(panel, "", font, 16, FontStyle.Normal, new Vector2(0, -20), new Vector2(260, 280), TextAnchor.UpperLeft, new Color(0.76f, 0.84f, 0.92f));

            AuraRayStatsPanel stats = panel.gameObject.AddComponent<AuraRayStatsPanel>();
            stats.Configure(title, body);
            return stats;
        }

        private static List<Button> CreateModeButtons(Transform parent, Font font)
        {
            RectTransform row = CreateRect(parent, "Mode Buttons", new Vector2(850, 70), new Vector2(0.5f, 0), new Vector2(0, 58));
            string[] labels = { "Full Quality", "Low Quality", "Gaze-Aware", "Overlay" };
            List<Button> buttons = new List<Button>();

            for (int i = 0; i < labels.Length; i++)
            {
                int index = i;
                Button button = CreateButton(row, labels[i], font, new Vector2(-315 + i * 210, 0), new Vector2(185, 46));
                button.onClick.AddListener(() =>
                {
                    AuraRayModeController controller = Object.FindFirstObjectByType<AuraRayModeController>();
                    if (controller != null)
                    {
                        controller.ApplyModeIndex(index);
                    }
                });
                buttons.Add(button);
            }

            return buttons;
        }

        private static RectTransform CreatePanel(Transform parent, string name, Vector2 anchor, Vector2 size, Vector2 position, Color color)
        {
            RectTransform rect = CreateRect(parent, name, size, anchor, position);
            Image image = rect.gameObject.AddComponent<Image>();
            image.color = color;
            return rect;
        }

        private static RectTransform CreateRect(Transform parent, string name, Vector2 size, Vector2 anchor, Vector2 position)
        {
            GameObject gameObject = new GameObject(name, typeof(RectTransform));
            RectTransform rect = gameObject.GetComponent<RectTransform>();
            rect.SetParent(parent, false);
            rect.anchorMin = anchor;
            rect.anchorMax = anchor;
            rect.pivot = new Vector2(0.5f, 0.5f);
            rect.sizeDelta = size;
            rect.anchoredPosition = position;
            return rect;
        }

        private static Text CreateText(Transform parent, string value, Font font, int size, FontStyle style, Vector2 position, Vector2 dimensions, TextAnchor alignment, Color color)
        {
            GameObject gameObject = new GameObject("Text", typeof(RectTransform), typeof(Text));
            RectTransform rect = gameObject.GetComponent<RectTransform>();
            rect.SetParent(parent, false);
            rect.sizeDelta = dimensions;
            rect.anchoredPosition = position;

            Text text = gameObject.GetComponent<Text>();
            text.text = value;
            text.font = font;
            text.fontSize = size;
            text.fontStyle = style;
            text.alignment = alignment;
            text.color = color;
            text.horizontalOverflow = HorizontalWrapMode.Wrap;
            text.verticalOverflow = VerticalWrapMode.Overflow;
            return text;
        }

        private static Button CreateButton(Transform parent, string label, Font font, Vector2 position, Vector2 size)
        {
            GameObject gameObject = new GameObject(label, typeof(RectTransform), typeof(Image), typeof(Button));
            RectTransform rect = gameObject.GetComponent<RectTransform>();
            rect.SetParent(parent, false);
            rect.sizeDelta = size;
            rect.anchoredPosition = position;

            Button button = gameObject.GetComponent<Button>();
            ColorBlock colors = button.colors;
            colors.normalColor = new Color(0.08f, 0.10f, 0.15f);
            colors.highlightedColor = new Color(0.22f, 0.54f, 0.96f);
            colors.pressedColor = new Color(0.10f, 0.28f, 0.55f);
            colors.selectedColor = new Color(0.17f, 0.46f, 0.82f);
            button.colors = colors;

            CreateText(rect, label, font, 16, FontStyle.Bold, Vector2.zero, size, TextAnchor.MiddleCenter, Color.white);
            return button;
        }
    }
}
