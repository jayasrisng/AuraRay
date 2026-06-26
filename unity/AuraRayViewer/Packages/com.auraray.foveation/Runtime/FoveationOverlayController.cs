using UnityEngine;
using UnityEngine.UI;

namespace AuraRay
{
    /// <summary>Draws a configurable screen-space visualization of gaze-aware quality regions.</summary>
    [RequireComponent(typeof(RawImage))]
    public class FoveationOverlayController : MonoBehaviour
    {
        private const int TextureWidth = 480;
        private const int TextureHeight = 270;
        private const float MinimumRadiusGap = 0.01f;

        [Header("Foveation Regions")]
        [SerializeField, Range(0.01f, 0.74f), Tooltip("Radius of the highest-quality region in normalized image height.")]
        private float fovealRadius = 0.18f;

        [SerializeField, Range(0.02f, 0.75f), Tooltip("Outer radius of the medium-quality transition region.")]
        private float transitionRadius = 0.36f;

        [Header("Sample Budget")]
        [SerializeField, Range(1, 256), Tooltip("Samples represented inside the foveal center.")]
        private int centerSamples = 64;

        [SerializeField, Range(1, 256), Tooltip("Samples represented inside the transition ring.")]
        private int middleSamples = 24;

        [SerializeField, Range(1, 256), Tooltip("Samples represented in the image periphery and Low Quality mode.")]
        private int peripherySamples = 8;

        [Header("Appearance")]
        [SerializeField, Range(0.0f, 1.0f), Tooltip("Global opacity multiplier for the generated overlay.")]
        private float overlayOpacity = 1.0f;

        [SerializeField, Tooltip("Show the foveal and transition boundary rings.")]
        private bool showRings = true;

        [SerializeField, Range(0.0f, 0.25f), Tooltip("Amount of deterministic visual noise added outside the foveal region.")]
        private float noiseStrength = 0.08f;

        [SerializeField, Range(0.0f, 1.0f), Tooltip("Strength of peripheral dimming in Low, Gaze-Aware, and Overlay modes.")]
        private float dimmingStrength = 1.0f;

        private RawImage image;
        private Texture2D texture;
        private FoveationMode activeMode = FoveationMode.GazeAware;
        private Vector2 activeGaze = new Vector2(0.5f, 0.5f);

        /// <summary>Radius of the high-quality foveal region.</summary>
        public float FovealRadius => fovealRadius;

        /// <summary>Outer radius of the medium-quality transition region.</summary>
        public float TransitionRadius => transitionRadius;

        /// <summary>Effective sample count represented at the gaze center for the active mode.</summary>
        public int CenterSamples => activeMode == FoveationMode.LowQuality ? peripherySamples : centerSamples;

        /// <summary>Effective sample count represented in the transition region for the active mode.</summary>
        public int MiddleSamples => activeMode == FoveationMode.FullQuality ? centerSamples : activeMode == FoveationMode.LowQuality ? peripherySamples : middleSamples;

        /// <summary>Effective sample count represented in the periphery for the active mode.</summary>
        public int PeripherySamples => activeMode == FoveationMode.FullQuality ? centerSamples : peripherySamples;

        /// <summary>Configured sample count used as the full-quality baseline.</summary>
        public int ConfiguredCenterSamples => centerSamples;

        private void OnValidate()
        {
            ValidateSettings();

            if (texture != null)
            {
                Redraw();
            }
        }

        private void Awake()
        {
            ValidateSettings();
            EnsureTexture();
        }

        /// <summary>Changes the active foveation visualization mode.</summary>
        public void SetMode(FoveationMode mode)
        {
            activeMode = mode;
            Redraw();
        }

        /// <summary>Moves the overlay to a normalized gaze position.</summary>
        public void SetGaze(Vector2 normalizedGaze)
        {
            normalizedGaze = new Vector2(Mathf.Clamp01(normalizedGaze.x), Mathf.Clamp01(normalizedGaze.y));

            if ((normalizedGaze - activeGaze).sqrMagnitude < 0.00002f)
            {
                return;
            }

            activeGaze = normalizedGaze;
            Redraw();
        }

        /// <summary>Estimates primary rays for the active mode at the reference 400 by 225 resolution.</summary>
        public long EstimatePrimaryRays()
        {
            long rays = 0;
            for (int y = 0; y < 225; y++)
            {
                for (int x = 0; x < 400; x++)
                {
                    float u = x / 399.0f;
                    float v = y / 224.0f;
                    rays += SamplesForDistance(FoveatedDistance(u, v));
                }
            }
            return rays;
        }

        private void EnsureTexture()
        {
            if (image == null)
            {
                image = GetComponent<RawImage>();
            }

            if (texture == null)
            {
                if (image == null)
                {
                    return;
                }

                texture = new Texture2D(TextureWidth, TextureHeight, TextureFormat.RGBA32, false);
                texture.name = "AuraRay Interactive Foveation Overlay";
                texture.wrapMode = TextureWrapMode.Clamp;
                texture.filterMode = FilterMode.Bilinear;
                image.texture = texture;
                image.color = Color.white;
                Redraw();
            }
        }

        private void Redraw()
        {
            EnsureTexture();

            if (texture == null)
            {
                return;
            }

            for (int y = 0; y < TextureHeight; y++)
            {
                for (int x = 0; x < TextureWidth; x++)
                {
                    float u = x / (TextureWidth - 1.0f);
                    float v = y / (TextureHeight - 1.0f);
                    float distance = FoveatedDistance(u, v);
                    Color color = PixelColor(distance, x, y);
                    texture.SetPixel(x, y, color);
                }
            }

            texture.Apply();
        }

        private Color PixelColor(float distance, int x, int y)
        {
            if (activeMode == FoveationMode.FullQuality)
            {
                return new Color(0, 0, 0, 0);
            }

            float noise = (((x * 17 + y * 37) % 23) / 22.0f) * noiseStrength;

            if (activeMode == FoveationMode.LowQuality)
            {
                return DimmedColor(0.03f + noise, 0.04f + noise, 0.06f + noise, 0.36f);
            }

            bool fovealRing = showRings && Mathf.Abs(distance - fovealRadius) < 0.007f;
            bool transitionRing = showRings && Mathf.Abs(distance - transitionRadius) < 0.008f;
            bool gazeDot = distance < 0.018f;

            if (activeMode == FoveationMode.Overlay)
            {
                if (gazeDot) return OverlayColor(1.0f, 0.16f, 0.54f, 0.95f);
                if (fovealRing) return OverlayColor(1.0f, 1.0f, 1.0f, 0.95f);
                if (transitionRing) return OverlayColor(0.35f, 0.78f, 1.0f, 0.85f);
                if (distance < fovealRadius) return DimmedColor(0.15f, 0.58f, 1.0f, 0.18f);
                if (distance < transitionRadius) return DimmedColor(0.05f, 0.30f, 0.60f, 0.28f);
                return DimmedColor(0.02f, 0.03f, 0.05f, 0.48f);
            }

            if (gazeDot) return OverlayColor(1.0f, 0.16f, 0.54f, 0.95f);
            if (fovealRing) return OverlayColor(1.0f, 1.0f, 1.0f, 0.82f);
            if (transitionRing) return OverlayColor(0.35f, 0.78f, 1.0f, 0.56f);
            if (distance < fovealRadius) return new Color(0, 0, 0, 0);
            if (distance < transitionRadius) return DimmedColor(0.06f + noise, 0.08f + noise, 0.11f + noise, 0.16f);
            return DimmedColor(0.04f + noise, 0.05f + noise, 0.07f + noise, 0.38f);
        }

        private int SamplesForDistance(float distance)
        {
            if (activeMode == FoveationMode.FullQuality) return centerSamples;
            if (activeMode == FoveationMode.LowQuality) return peripherySamples;
            if (distance <= fovealRadius) return centerSamples;
            if (distance <= transitionRadius) return middleSamples;
            return peripherySamples;
        }

        private Color OverlayColor(float red, float green, float blue, float alpha)
        {
            return new Color(red, green, blue, alpha * overlayOpacity);
        }

        private Color DimmedColor(float red, float green, float blue, float alpha)
        {
            return OverlayColor(red, green, blue, alpha * dimmingStrength);
        }

        private void ValidateSettings()
        {
            fovealRadius = Mathf.Clamp(fovealRadius, 0.01f, 0.74f);
            transitionRadius = Mathf.Clamp(transitionRadius, fovealRadius + MinimumRadiusGap, 0.75f);
            centerSamples = Mathf.Max(1, centerSamples);
            middleSamples = Mathf.Max(1, middleSamples);
            peripherySamples = Mathf.Max(1, peripherySamples);
            overlayOpacity = Mathf.Clamp01(overlayOpacity);
            noiseStrength = Mathf.Clamp(noiseStrength, 0.0f, 0.25f);
            dimmingStrength = Mathf.Clamp01(dimmingStrength);
            activeGaze = new Vector2(Mathf.Clamp01(activeGaze.x), Mathf.Clamp01(activeGaze.y));
        }

        private float FoveatedDistance(float u, float v)
        {
            float dx = (u - activeGaze.x) * (16.0f / 9.0f);
            float dy = v - activeGaze.y;
            return Mathf.Sqrt(dx * dx + dy * dy);
        }
    }
}
