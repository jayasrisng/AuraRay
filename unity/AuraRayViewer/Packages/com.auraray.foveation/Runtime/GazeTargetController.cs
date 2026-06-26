using UnityEngine;

namespace AuraRay
{
    public class GazeTargetController : MonoBehaviour
    {
        [SerializeField] private Transform gazeTarget;
        [SerializeField] private Camera sceneCamera;
        [SerializeField] private float moveSpeed = 0.42f;
        [SerializeField] private Vector2 normalizedGaze = new Vector2(0.5f, 0.5f);

        public Vector2 NormalizedGaze => normalizedGaze;

        private void Update()
        {
            Vector2 delta = Vector2.zero;

            if (Input.GetKey(KeyCode.LeftArrow) || Input.GetKey(KeyCode.A)) delta.x -= 1.0f;
            if (Input.GetKey(KeyCode.RightArrow) || Input.GetKey(KeyCode.D)) delta.x += 1.0f;
            if (Input.GetKey(KeyCode.DownArrow) || Input.GetKey(KeyCode.S)) delta.y -= 1.0f;
            if (Input.GetKey(KeyCode.UpArrow) || Input.GetKey(KeyCode.W)) delta.y += 1.0f;

            if (delta.sqrMagnitude > 0.0f)
            {
                normalizedGaze += delta.normalized * moveSpeed * Time.deltaTime;
                normalizedGaze.x = Mathf.Clamp01(normalizedGaze.x);
                normalizedGaze.y = Mathf.Clamp01(normalizedGaze.y);
                ApplyTargetPosition();
            }

            if (Input.GetMouseButtonDown(0) && sceneCamera != null)
            {
                Vector3 mouse = Input.mousePosition;
                normalizedGaze = new Vector2(mouse.x / Screen.width, mouse.y / Screen.height);
                normalizedGaze.x = Mathf.Clamp01(normalizedGaze.x);
                normalizedGaze.y = Mathf.Clamp01(normalizedGaze.y);
                ApplyTargetPosition();
            }
        }

        public void Configure(Transform target, Camera camera)
        {
            gazeTarget = target;
            sceneCamera = camera;
            ApplyTargetPosition();
        }

        public void SetNormalizedGaze(Vector2 value)
        {
            normalizedGaze = new Vector2(Mathf.Clamp01(value.x), Mathf.Clamp01(value.y));
            ApplyTargetPosition();
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
