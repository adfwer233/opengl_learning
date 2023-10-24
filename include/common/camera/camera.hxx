#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr float default_pitch = 0.0f;
constexpr float default_yaw = -90.0f;

class Camera {
    private:

    public:
        // position of camera in world coordinate
        glm::vec3 position;

        // position of scene origin in world space
        glm::vec3 camera_target;

        // up direction of camera (normalized)
        glm::vec3 camera_up_axis;

        // right axis direction of camera (normalized)
        glm::vec3 camera_right_axis;

        // euler angle
        float yaw;
        float pitch;

        // camera options
        float zoom;
        const float move_speed;
        const float mouse_sensitivity;

        Camera(glm::vec3 pos, glm::vec3 up, float yaw = default_yaw, float pitch = default_pitch): zoom(45), move_speed(2.5), mouse_sensitivity(0.1f) {
            position = pos;
            camera_up_axis = up;
        }

        glm::mat4 get_view_transformation();
};