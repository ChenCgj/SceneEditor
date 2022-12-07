#ifndef CAMERA_H
#define CAMERA_H
#include "glm.hpp"

class Camera {
public:
    Camera();
    Camera(const glm::vec3 &target, const glm::vec3 &up, const glm::vec3 &pos, float ratio = 16 / 9.0,
        float fov = glm::radians(45.0f), float near = 0.1f, float far = 10.0f);

    void set_pos(const glm::vec3 &pos);
    void set_pos(const glm::vec3 &pos, const glm::vec3 &target);
    const glm::vec3 &get_pos() const;
    void set_target(const glm::vec3 &target);
    const glm::vec3 &get_front() const;
    void move(const glm::vec3 &delta);
    void rotate(float rad, const glm::vec3 &axis);
    void set_distance(float near, float far);
    void set_fov(float rad);
    float get_fov() const;
    void set_size_ratio(float ratio);
    float get_size_ratio() const;
    glm::mat4 get_camera_matrix() const;
    glm::mat4 get_project_matrix() const;
    void zoom(float times);
private:
    glm::vec3 dir;
    glm::vec3 up;
    glm::vec3 pos;
    float near, far;
    float fov;
    float size_ratio;
    // glm::vec3 target;
};
#endif