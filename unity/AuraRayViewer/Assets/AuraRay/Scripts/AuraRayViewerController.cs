using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace AuraRay
{
    public class AuraRayViewerController : MonoBehaviour
    {
        private readonly List<RenderMode> modes = new List<RenderMode>
        {
            new RenderMode("Full Quality", "foveated_full"),
            new RenderMode("Low Quality", "foveated_low"),
            new RenderMode("Gaze-Aware", "foveated_gaze"),
            new RenderMode("Overlay / Heatmap", "foveated_overlay"),
        };

        [SerializeField] private RawImage renderImage;
        [SerializeField] private AuraRayGazeOverlay gazeOverlay;
        [SerializeField] private Text modeTitle;
        [SerializeField] private Text metadataText;
        [SerializeField] private List<Button> buttons = new List<Button>();
        [SerializeField] private int currentMode;
        [SerializeField] private bool interfaceBuilt;

        private void Start()
        {
            if (!interfaceBuilt)
            {
                BuildInterface();
            }

            ShowMode(0);
        }

        private void Update()
        {
            if (Input.GetKeyDown(KeyCode.Alpha1)) ShowMode(0);
            if (Input.GetKeyDown(KeyCode.Alpha2)) ShowMode(1);
            if (Input.GetKeyDown(KeyCode.Alpha3)) ShowMode(2);
            if (Input.GetKeyDown(KeyCode.Alpha4)) ShowMode(3);
            if (Input.GetKeyDown(KeyCode.RightArrow)) ShowMode((currentMode + 1) % modes.Count);
            if (Input.GetKeyDown(KeyCode.LeftArrow)) ShowMode((currentMode + modes.Count - 1) % modes.Count);
        }

        public void BuildInterface()
        {
            if (interfaceBuilt)
            {
                return;
            }

            Camera.main.backgroundColor = new Color(0.015f, 0.018f, 0.028f);

            Canvas canvas = CreateCanvas();
            Font font = Resources.GetBuiltinResource<Font>("LegacyRuntime.ttf");

            RectTransform root = CreatePanel(canvas.transform, "AuraRay Floating Display", new Vector2(0.5f, 0.5f), new Vector2(1180, 720), new Color(0.035f, 0.043f, 0.065f, 0.96f));
            CreateText(root, "AuraRay", font, 44, FontStyle.Bold, new Vector2(-340, 300), new Vector2(420, 60), TextAnchor.MiddleLeft, new Color(0.92f, 0.96f, 1.0f));
            CreateText(root, "Gaze-Aware Ray Tracing Viewer", font, 20, FontStyle.Normal, new Vector2(-340, 258), new Vector2(500, 36), TextAnchor.MiddleLeft, new Color(0.56f, 0.74f, 0.92f));

            RectTransform imageFrame = CreatePanel(root, "Image Frame", new Vector2(0.42f, 0.47f), new Vector2(820, 500), new Color(0.012f, 0.016f, 0.026f, 1.0f));
            renderImage = CreateRawImage(imageFrame, "Render Image", new Vector2(0.5f, 0.5f), new Vector2(760, 428));
            gazeOverlay = CreateOverlay(renderImage.transform, "Gaze Overlay");

            RectTransform sidePanel = CreatePanel(root, "Metadata Panel", new Vector2(0.86f, 0.49f), new Vector2(300, 500), new Color(0.055f, 0.065f, 0.088f, 0.98f));
            modeTitle = CreateText(sidePanel, "", font, 24, FontStyle.Bold, new Vector2(0, 198), new Vector2(250, 40), TextAnchor.MiddleLeft, new Color(0.95f, 0.98f, 1.0f));
            metadataText = CreateText(sidePanel, "", font, 16, FontStyle.Normal, new Vector2(0, 35), new Vector2(250, 280), TextAnchor.UpperLeft, new Color(0.78f, 0.86f, 0.94f));

            RectTransform buttonRow = CreateRect(root, "Mode Buttons", new Vector2(0.42f, 0.08f), new Vector2(820, 70));
            for (int i = 0; i < modes.Count; i++)
            {
                int index = i;
                Button button = CreateButton(buttonRow, modes[i].Label, font, new Vector2(-307 + i * 205, 0), new Vector2(185, 46));
                button.onClick.AddListener(() => ShowMode(index));
                buttons.Add(button);
            }

            interfaceBuilt = true;
        }

        public void ShowMode(int index)
        {
            currentMode = Mathf.Clamp(index, 0, modes.Count - 1);
            RenderMode mode = modes[currentMode];

            Texture2D texture = Resources.Load<Texture2D>($"Renders/{mode.ResourceName}");
            TextAsset metadataAsset = Resources.Load<TextAsset>($"Renders/{mode.ResourceName}");
            AuraRayMetadata metadata = metadataAsset != null
                ? JsonUtility.FromJson<AuraRayMetadata>(metadataAsset.text)
                : null;

            renderImage.texture = texture;
            modeTitle.text = mode.Label;
            metadataText.text = FormatMetadata(metadata);
            gazeOverlay.SetMetadata(metadata);
            gazeOverlay.gameObject.SetActive(mode.ResourceName != "foveated_full" && mode.ResourceName != "foveated_low");

            for (int i = 0; i < buttons.Count; i++)
            {
                ColorBlock colors = buttons[i].colors;
                colors.normalColor = i == currentMode ? new Color(0.18f, 0.44f, 0.78f) : new Color(0.09f, 0.12f, 0.18f);
                colors.highlightedColor = new Color(0.22f, 0.50f, 0.88f);
                buttons[i].colors = colors;
            }
        }

        public string CurrentModeLabel => modeTitle != null ? modeTitle.text : string.Empty;

        public string CurrentMetadataText => metadataText != null ? metadataText.text : string.Empty;

        private static string FormatMetadata(AuraRayMetadata metadata)
        {
            if (metadata == null)
            {
                return "Metadata unavailable.";
            }

            return
                $"render mode\n{metadata.render_mode}\n\n" +
                $"estimated rays\n{metadata.estimated_total_rays:n0}\n\n" +
                $"render time\n{metadata.render_time_ms} ms\n\n" +
                $"samples\ncenter {metadata.samples_center} / middle {metadata.samples_middle} / periphery {metadata.samples_periphery}\n\n" +
                $"gaze\n({metadata.gaze_x:0.00}, {metadata.gaze_y:0.00})\n\n" +
                $"scene\n{metadata.scene_name}";
        }

        private static Canvas CreateCanvas()
        {
            GameObject canvasObject = new GameObject("AuraRay Viewer Canvas", typeof(Canvas), typeof(CanvasScaler), typeof(GraphicRaycaster));
            Canvas canvas = canvasObject.GetComponent<Canvas>();
            canvas.renderMode = UnityEngine.RenderMode.ScreenSpaceCamera;
            canvas.worldCamera = Camera.main;
            canvas.planeDistance = 5.0f;
            CanvasScaler scaler = canvasObject.GetComponent<CanvasScaler>();
            scaler.uiScaleMode = CanvasScaler.ScaleMode.ScaleWithScreenSize;
            scaler.referenceResolution = new Vector2(1440, 900);
            scaler.matchWidthOrHeight = 0.5f;
            return canvas;
        }

        private static RectTransform CreatePanel(Transform parent, string name, Vector2 anchor, Vector2 size, Color color)
        {
            RectTransform rect = CreateRect(parent, name, anchor, size);
            Image image = rect.gameObject.AddComponent<Image>();
            image.color = color;
            return rect;
        }

        private static RectTransform CreateRect(Transform parent, string name, Vector2 anchor, Vector2 size)
        {
            GameObject gameObject = new GameObject(name, typeof(RectTransform));
            RectTransform rect = gameObject.GetComponent<RectTransform>();
            rect.SetParent(parent, false);
            rect.anchorMin = anchor;
            rect.anchorMax = anchor;
            rect.pivot = new Vector2(0.5f, 0.5f);
            rect.sizeDelta = size;
            rect.anchoredPosition = Vector2.zero;
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

        private static RawImage CreateRawImage(Transform parent, string name, Vector2 anchor, Vector2 size)
        {
            GameObject gameObject = new GameObject(name, typeof(RectTransform), typeof(RawImage));
            RectTransform rect = gameObject.GetComponent<RectTransform>();
            rect.SetParent(parent, false);
            rect.anchorMin = anchor;
            rect.anchorMax = anchor;
            rect.pivot = new Vector2(0.5f, 0.5f);
            rect.sizeDelta = size;
            rect.anchoredPosition = Vector2.zero;

            RawImage image = gameObject.GetComponent<RawImage>();
            image.color = Color.white;
            return image;
        }

        private static AuraRayGazeOverlay CreateOverlay(Transform parent, string name)
        {
            GameObject gameObject = new GameObject(name, typeof(RectTransform), typeof(RawImage), typeof(AuraRayGazeOverlay));
            RectTransform rect = gameObject.GetComponent<RectTransform>();
            rect.SetParent(parent, false);
            rect.anchorMin = Vector2.zero;
            rect.anchorMax = Vector2.one;
            rect.offsetMin = Vector2.zero;
            rect.offsetMax = Vector2.zero;
            return gameObject.GetComponent<AuraRayGazeOverlay>();
        }

        private static Button CreateButton(Transform parent, string label, Font font, Vector2 position, Vector2 size)
        {
            GameObject gameObject = new GameObject(label, typeof(RectTransform), typeof(Image), typeof(Button));
            RectTransform rect = gameObject.GetComponent<RectTransform>();
            rect.SetParent(parent, false);
            rect.sizeDelta = size;
            rect.anchoredPosition = position;

            Image image = gameObject.GetComponent<Image>();
            image.color = new Color(0.09f, 0.12f, 0.18f);

            Button button = gameObject.GetComponent<Button>();
            ColorBlock colors = button.colors;
            colors.normalColor = image.color;
            colors.highlightedColor = new Color(0.22f, 0.50f, 0.88f);
            colors.pressedColor = new Color(0.10f, 0.28f, 0.55f);
            colors.selectedColor = new Color(0.18f, 0.44f, 0.78f);
            button.colors = colors;

            CreateText(rect, label, font, 16, FontStyle.Bold, Vector2.zero, size, TextAnchor.MiddleCenter, Color.white);
            return button;
        }

        [Serializable]
        private struct RenderMode
        {
            public string Label;
            public string ResourceName;

            public RenderMode(string label, string resourceName)
            {
                Label = label;
                ResourceName = resourceName;
            }
        }
    }
}
