using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace AuraRay
{
    public class AuraRayModeController : MonoBehaviour
    {
        [SerializeField] private GazeTargetController gazeTarget;
        [SerializeField] private FoveationOverlayController overlay;
        [SerializeField] private AuraRayStatsPanel statsPanel;
        [SerializeField] private List<Button> modeButtons = new List<Button>();
        [SerializeField] private FoveationMode currentMode = FoveationMode.GazeAware;

        private Vector2 lastGaze = new Vector2(-1, -1);

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

        public void Configure(GazeTargetController gaze, FoveationOverlayController foveationOverlay, AuraRayStatsPanel stats, List<Button> buttons)
        {
            gazeTarget = gaze;
            overlay = foveationOverlay;
            statsPanel = stats;
            modeButtons = buttons;
            ApplyMode(FoveationMode.GazeAware);
        }

        public void ApplyMode(FoveationMode mode)
        {
            currentMode = mode;
            Refresh();
            UpdateButtonColors();
        }

        public void ApplyModeIndex(int index)
        {
            ApplyMode((FoveationMode)Mathf.Clamp(index, 0, 3));
        }

        private void Refresh()
        {
            if (gazeTarget == null || overlay == null || statsPanel == null)
            {
                return;
            }

            lastGaze = gazeTarget.NormalizedGaze;
            overlay.SetMode(currentMode);
            overlay.SetGaze(lastGaze);
            statsPanel.UpdateStats(currentMode, lastGaze, overlay.CenterSamples, overlay.MiddleSamples, overlay.PeripherySamples, overlay.EstimatePrimaryRays());
        }

        private void UpdateButtonColors()
        {
            for (int i = 0; i < modeButtons.Count; i++)
            {
                ColorBlock colors = modeButtons[i].colors;
                colors.normalColor = i == (int)currentMode ? new Color(0.17f, 0.46f, 0.82f) : new Color(0.08f, 0.10f, 0.15f);
                colors.highlightedColor = new Color(0.22f, 0.54f, 0.96f);
                colors.selectedColor = colors.normalColor;
                modeButtons[i].colors = colors;
            }
        }
    }
}
