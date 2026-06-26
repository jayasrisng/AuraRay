using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace AuraRay
{
    /// <summary>Coordinates gaze, overlay, statistics, and controls for the active foveation mode.</summary>
    public class AuraRayModeController : MonoBehaviour
    {
        [SerializeField, Tooltip("Provider that supplies the current normalized gaze position.")]
        private GazeTargetController gazeTarget;

        [SerializeField, Tooltip("Overlay updated when the gaze point or quality mode changes.")]
        private FoveationOverlayController overlay;

        [SerializeField, Tooltip("Optional panel used to display gaze and sample budget statistics.")]
        private AuraRayStatsPanel statsPanel;

        [SerializeField, Tooltip("Optional buttons ordered as Full, Low, Gaze-Aware, and Overlay.")]
        private List<Button> modeButtons = new List<Button>();

        [SerializeField, Tooltip("Quality mode applied when the scene starts.")]
        private FoveationMode currentMode = FoveationMode.GazeAware;

        private Vector2 lastGaze = new Vector2(-1, -1);

        /// <summary>Currently active visualization mode.</summary>
        public FoveationMode CurrentMode => currentMode;

        private void Start()
        {
            ApplyMode(currentMode);
        }

        private void Update()
        {
            if (Input.GetKeyDown(KeyCode.Alpha1)) ApplyMode(FoveationMode.FullQuality);
            if (Input.GetKeyDown(KeyCode.Alpha2)) ApplyMode(FoveationMode.LowQuality);
            if (Input.GetKeyDown(KeyCode.Alpha3)) ApplyMode(FoveationMode.GazeAware);
            if (Input.GetKeyDown(KeyCode.Alpha4)) ApplyMode(FoveationMode.Overlay);

            if (gazeTarget != null && (gazeTarget.NormalizedGaze - lastGaze).sqrMagnitude > 0.00002f)
            {
                Refresh();
            }
        }

        /// <summary>Connects the controller to a gaze provider, overlay, optional stats panel, and optional mode buttons.</summary>
        public void Configure(GazeTargetController gaze, FoveationOverlayController foveationOverlay, AuraRayStatsPanel stats, List<Button> buttons)
        {
            gazeTarget = gaze;
            overlay = foveationOverlay;
            statsPanel = stats;
            modeButtons = buttons ?? new List<Button>();
            ApplyMode(currentMode);
        }

        /// <summary>Applies a quality mode and refreshes all connected views.</summary>
        public void ApplyMode(FoveationMode mode)
        {
            currentMode = mode;
            Refresh();
            UpdateButtonColors();
        }

        /// <summary>Applies a mode by its zero-based enum index.</summary>
        public void ApplyModeIndex(int index)
        {
            ApplyMode((FoveationMode)Mathf.Clamp(index, 0, 3));
        }

        private void Refresh()
        {
            if (gazeTarget == null || overlay == null)
            {
                return;
            }

            lastGaze = gazeTarget.NormalizedGaze;
            overlay.SetMode(currentMode);
            overlay.SetGaze(lastGaze);
            if (statsPanel != null)
            {
                statsPanel.UpdateStats(currentMode, lastGaze, overlay.CenterSamples, overlay.MiddleSamples, overlay.PeripherySamples, overlay.EstimatePrimaryRays(), overlay.ConfiguredCenterSamples);
            }
        }

        private void UpdateButtonColors()
        {
            for (int i = 0; i < modeButtons.Count; i++)
            {
                if (modeButtons[i] == null)
                {
                    continue;
                }

                ColorBlock colors = modeButtons[i].colors;
                colors.normalColor = i == (int)currentMode ? new Color(0.17f, 0.46f, 0.82f) : new Color(0.08f, 0.10f, 0.15f);
                colors.highlightedColor = new Color(0.22f, 0.54f, 0.96f);
                colors.selectedColor = colors.normalColor;
                modeButtons[i].colors = colors;
            }
        }
    }
}
