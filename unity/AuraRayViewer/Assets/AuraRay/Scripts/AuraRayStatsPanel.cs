using UnityEngine;
using UnityEngine.UI;

namespace AuraRay
{
    public class AuraRayStatsPanel : MonoBehaviour
    {
        [SerializeField] private Text titleText;
        [SerializeField] private Text bodyText;

        public void Configure(Text title, Text body)
        {
            titleText = title;
            bodyText = body;
        }

        public void UpdateStats(FoveationMode mode, Vector2 gaze, int centerSamples, int middleSamples, int peripherySamples, long estimatedRays)
        {
            if (titleText != null)
            {
                titleText.text = "Interactive Gaze-Aware";
            }

            if (bodyText == null)
            {
                return;
            }

            const long fullQualityRays = 400L * 225L * 64L;
            float percent = estimatedRays / (float)fullQualityRays * 100.0f;

            bodyText.text =
                $"mode\n{ModeLabel(mode)}\n\n" +
                $"gaze x/y\n{gaze.x:0.00}, {gaze.y:0.00}\n\n" +
                $"samples\ncenter {centerSamples} / middle {middleSamples} / periphery {peripherySamples}\n\n" +
                $"estimated rays\n{estimatedRays:n0} ({percent:0.0}% of full)\n\n" +
                "Move the gaze target to shift the high-quality region.";
        }

        private static string ModeLabel(FoveationMode mode)
        {
            switch (mode)
            {
                case FoveationMode.FullQuality:
                    return "Full Quality";
                case FoveationMode.LowQuality:
                    return "Low Quality";
                case FoveationMode.Overlay:
                    return "Overlay";
                default:
                    return "Gaze-Aware";
            }
        }
    }
}
