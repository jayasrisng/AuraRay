using System;

namespace AuraRay
{
    [Serializable]
    public class AuraRayMetadata
    {
        public string render_mode;
        public string scene_name;
        public string output_ppm;
        public string output_png;
        public int image_width;
        public int image_height;
        public float gaze_x;
        public float gaze_y;
        public float foveal_radius;
        public float transition_radius;
        public int samples_center;
        public int samples_middle;
        public int samples_periphery;
        public long estimated_total_rays;
        public int render_time_ms;
        public string[] materials_used;
        public string camera_description;
    }
}
