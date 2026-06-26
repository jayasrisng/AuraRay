using UnityEngine;
using UnityEngine.UI;

namespace AuraRay
{
    [RequireComponent(typeof(RawImage))]
    public class AuraRayGazeOverlay : MonoBehaviour
    {
        private const int Width = 760;
        private const int Height = 428;

        private RawImage image;
        private Texture2D texture;

        private void Awake()
        {
            EnsureTexture();
        }

        public void SetMetadata(AuraRayMetadata metadata)
        {
            if (metadata == null)
            {
                return;
            }

            EnsureTexture();
            Clear();

            DrawFilledCircle(metadata.gaze_x, metadata.gaze_y, 7, new Color(1.0f, 0.18f, 0.58f, 0.95f));
            DrawRing(metadata.gaze_x, metadata.gaze_y, metadata.transition_radius, 3, new Color(0.46f, 0.76f, 1.0f, 0.78f));
            DrawRing(metadata.gaze_x, metadata.gaze_y, metadata.foveal_radius, 3, new Color(1.0f, 1.0f, 1.0f, 0.9f));

            texture.Apply();
        }

        private void EnsureTexture()
        {
            if (image == null)
            {
                image = GetComponent<RawImage>();
            }

            if (texture == null)
            {
                texture = new Texture2D(Width, Height, TextureFormat.RGBA32, false);
                texture.name = "AuraRay Gaze Overlay";
                image.texture = texture;
                image.color = Color.white;
                Clear();
                texture.Apply();
            }
        }

        private void Clear()
        {
            Color clear = new Color(0, 0, 0, 0);
            Color[] pixels = texture.GetPixels();
            for (int i = 0; i < pixels.Length; i++)
            {
                pixels[i] = clear;
            }
            texture.SetPixels(pixels);
        }

        private void DrawRing(float gazeX, float gazeY, float radius, int thickness, Color color)
        {
            float centerX = gazeX * (Width - 1);
            float centerY = gazeY * (Height - 1);
            float radiusPixels = radius * Height;
            float inner = radiusPixels - thickness;
            float outer = radiusPixels + thickness;

            for (int y = 0; y < Height; y++)
            {
                for (int x = 0; x < Width; x++)
                {
                    float dx = x - centerX;
                    float dy = y - centerY;
                    float distance = Mathf.Sqrt(dx * dx + dy * dy);
                    if (distance >= inner && distance <= outer)
                    {
                        texture.SetPixel(x, y, color);
                    }
                }
            }
        }

        private void DrawFilledCircle(float gazeX, float gazeY, int radius, Color color)
        {
            int centerX = Mathf.RoundToInt(gazeX * (Width - 1));
            int centerY = Mathf.RoundToInt(gazeY * (Height - 1));
            int radiusSquared = radius * radius;

            for (int y = centerY - radius; y <= centerY + radius; y++)
            {
                for (int x = centerX - radius; x <= centerX + radius; x++)
                {
                    if (x < 0 || x >= Width || y < 0 || y >= Height)
                    {
                        continue;
                    }

                    int dx = x - centerX;
                    int dy = y - centerY;
                    if (dx * dx + dy * dy <= radiusSquared)
                    {
                        texture.SetPixel(x, y, color);
                    }
                }
            }
        }
    }
}
