using UnityEngine;
using UnityEngine.UI;

namespace AuraRay
{
    /// <summary>Displays the current gaze, quality mode, sample tiers, and estimated ray budget.</summary>
    public class AuraRayStatsPanel : MonoBehaviour
    {
        [SerializeField, Tooltip("Optional heading for the statistics panel.")]
        private Text titleText;

        [SerializeField, Tooltip("Optional text element that receives formatted simulator statistics.")]
        private Text bodyText;

        /// <summary>Assigns optional UGUI text elements used by the panel.</summary>
        public void Configure(Text title, Text body)
        {
            titleText = title;
            bodyText = body;
        }

        /// <summary>Refreshes the panel with current mode, gaze, and sample budget values.</summary>
        public void UpdateStats(FoveationMode mode, Vector2 gaze, int centerSamples, int middleSamples, int peripherySamples, long estimatedRays, int fullQualitySamples = 64)
        {
            if (titleText != null)
            {
                titleText.text = "Interactive Gaze-Aware";
            }

            if (bodyText == null)
            {
                return;
            }

            long fullQualityRays = 400L * 225L * Mathf.Max(1, fullQualitySamples);
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
