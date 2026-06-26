using UnityEngine;
using UnityEngine.UI;

namespace AuraRay
{
    [RequireComponent(typeof(RawImage))]
    public class FoveationOverlayController : MonoBehaviour
    {
        private const int TextureWidth = 480;
        private const int TextureHeight = 270;

        [SerializeField] private float fovealRadius = 0.18f;
        [SerializeField] private float transitionRadius = 0.36f;

        private RawImage image;
        private Texture2D texture;
        private FoveationMode activeMode = FoveationMode.GazeAware;
        private Vector2 activeGaze = new Vector2(0.5f, 0.5f);

        public float FovealRadius => fovealRadius;
        public float TransitionRadius => transitionRadius;

        private void Awake()
        {
            EnsureTexture();
        }

        public void SetMode(FoveationMode mode)
        {
            activeMode = mode;
            Redraw();
        }

        public void SetGaze(Vector2 normalizedGaze)
        {
            if ((normalizedGaze - activeGaze).sqrMagnitude < 0.00002f)
            {
                return;
            }

            activeGaze = normalizedGaze;
            Redraw();
        }

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

        public int CenterSamples => activeMode == FoveationMode.LowQuality ? 8 : 64;
        public int MiddleSamples => activeMode == FoveationMode.FullQuality ? 64 : activeMode == FoveationMode.LowQuality ? 8 : 24;
        public int PeripherySamples => activeMode == FoveationMode.FullQuality ? 64 : 8;

        private void EnsureTexture()
        {
            if (image == null)
            {
                image = GetComponent<RawImage>();
            }

            if (texture == null)
            {
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

            float noise = (((x * 17 + y * 37) % 23) / 22.0f) * 0.08f;

            if (activeMode == FoveationMode.LowQuality)
            {
                return new Color(0.03f + noise, 0.04f + noise, 0.06f + noise, 0.36f);
            }

            bool fovealRing = Mathf.Abs(distance - fovealRadius) < 0.007f;
            bool transitionRing = Mathf.Abs(distance - transitionRadius) < 0.008f;
            bool gazeDot = distance < 0.018f;

            if (activeMode == FoveationMode.Overlay)
            {
                if (gazeDot) return new Color(1.0f, 0.16f, 0.54f, 0.95f);
                if (fovealRing) return new Color(1.0f, 1.0f, 1.0f, 0.95f);
                if (transitionRing) return new Color(0.35f, 0.78f, 1.0f, 0.85f);
                if (distance < fovealRadius) return new Color(0.15f, 0.58f, 1.0f, 0.18f);
                if (distance < transitionRadius) return new Color(0.05f, 0.30f, 0.60f, 0.28f);
                return new Color(0.02f, 0.03f, 0.05f, 0.48f);
            }

            if (gazeDot) return new Color(1.0f, 0.16f, 0.54f, 0.95f);
            if (fovealRing) return new Color(1.0f, 1.0f, 1.0f, 0.82f);
            if (transitionRing) return new Color(0.35f, 0.78f, 1.0f, 0.56f);
            if (distance < fovealRadius) return new Color(0, 0, 0, 0);
            if (distance < transitionRadius) return new Color(0.06f + noise, 0.08f + noise, 0.11f + noise, 0.16f);
            return new Color(0.04f + noise, 0.05f + noise, 0.07f + noise, 0.38f);
        }

        private int SamplesForDistance(float distance)
        {
            if (activeMode == FoveationMode.FullQuality) return 64;
            if (activeMode == FoveationMode.LowQuality) return 8;
            if (distance <= fovealRadius) return 64;
            if (distance <= transitionRadius) return 24;
            return 8;
        }

        private float FoveatedDistance(float u, float v)
        {
            float dx = (u - activeGaze.x) * (16.0f / 9.0f);
            float dy = v - activeGaze.y;
            return Mathf.Sqrt(dx * dx + dy * dy);
        }
    }
}
