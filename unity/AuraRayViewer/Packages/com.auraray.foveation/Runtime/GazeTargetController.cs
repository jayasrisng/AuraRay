using UnityEngine;

namespace AuraRay
{
    /// <summary>Provides a normalized simulated gaze point and moves a visible target to match it.</summary>
    public class GazeTargetController : MonoBehaviour
    {
        [SerializeField, Tooltip("Visible scene object moved to represent the current gaze point.")]
        private Transform gazeTarget;

        [SerializeField, Tooltip("Camera used to convert mouse clicks into normalized screen coordinates.")]
        private Camera sceneCamera;

        [SerializeField, Range(0.01f, 2.0f), Tooltip("Keyboard gaze movement speed in normalized units per second.")]
        private float moveSpeed = 0.42f;

        [SerializeField, Tooltip("Current gaze position in normalized screen coordinates.")]
        private Vector2 normalizedGaze = new Vector2(0.5f, 0.5f);

        /// <summary>Current gaze position with both axes clamped from zero to one.</summary>
        public Vector2 NormalizedGaze => normalizedGaze;

        /// <summary>Visible transform controlled by this gaze provider.</summary>
        public Transform GazeTarget => gazeTarget;

        /// <summary>Keyboard movement speed in normalized units per second.</summary>
        public float MoveSpeed => moveSpeed;

        private void OnValidate()
        {
            moveSpeed = Mathf.Max(0.01f, moveSpeed);
            normalizedGaze = ClampGaze(normalizedGaze);
        }

        private void Update()
        {
            Vector2 delta = Vector2.zero;

            if (Input.GetKey(KeyCode.LeftArrow) || Input.GetKey(KeyCode.A)) delta.x -= 1.0f;
            if (Input.GetKey(KeyCode.RightArrow) || Input.GetKey(KeyCode.D)) delta.x += 1.0f;
            if (Input.GetKey(KeyCode.DownArrow) || Input.GetKey(KeyCode.S)) delta.y -= 1.0f;
            if (Input.GetKey(KeyCode.UpArrow) || Input.GetKey(KeyCode.W)) delta.y += 1.0f;

            if (delta.sqrMagnitude > 0.0f)
            {
                normalizedGaze = ClampGaze(normalizedGaze + delta.normalized * moveSpeed * Time.deltaTime);
                ApplyTargetPosition();
            }

            if (Input.GetMouseButtonDown(0) && sceneCamera != null && Screen.width > 0 && Screen.height > 0)
            {
                Vector3 mouse = Input.mousePosition;
                normalizedGaze = ClampGaze(new Vector2(mouse.x / Screen.width, mouse.y / Screen.height));
                ApplyTargetPosition();
            }
        }

        /// <summary>Assigns the visible target and camera used by the simulated gaze provider.</summary>
        public void Configure(Transform target, Camera camera)
        {
            gazeTarget = target;
            sceneCamera = camera;
            ApplyTargetPosition();
        }

        /// <summary>Moves the simulated gaze to a normalized screen position.</summary>
        public void SetNormalizedGaze(Vector2 value)
        {
            normalizedGaze = ClampGaze(value);
            ApplyTargetPosition();
        }

        private static Vector2 ClampGaze(Vector2 value)
        {
            return new Vector2(Mathf.Clamp01(value.x), Mathf.Clamp01(value.y));
        }

        private void ApplyTargetPosition()
        {
            if (gazeTarget == null)
            {
                return;
            }

            float x = Mathf.Lerp(-2.2f, 2.2f, normalizedGaze.x);
            float y = Mathf.Lerp(0.35f, 2.35f, normalizedGaze.y);
            gazeTarget.position = new Vector3(x, y, 0.2f);
        }
    }
}
