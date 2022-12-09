#include <iostream>
#include "gtc/matrix_transform.hpp"
#include "camera.h"

Camera::Camera() : dir(glm::normalize(glm::vec3(0.0f, 0.0f, 5.0f))), 
    up(0.0f, 1.0f, 0.0f), pos(0.0f, 0.0f, 5.0f), near(0.1f), far(100.0f), fov(glm::radians(45.0f)), size_ratio(16 / 9.0) {}

Camera::Camera(const glm::vec3 &target, const glm::vec3 &up, const glm::vec3 &pos, float ratio, float fov, float near, float far) :
    dir(glm::normalize(pos - target)), up(up), pos(pos), near(near), far(far), fov(fov), size_ratio(ratio) {}

void Camera::set_pos(const glm::vec3 &pos)
{
    this->pos = pos;
}

void Camera::set_pos(const glm::vec3 &pos, const glm::vec3 &target)
{
    this->pos = pos;
    dir = glm::normalize(pos - target);
}

const glm::vec3 &Camera::get_pos() const
{
    return pos;
}

void Camera::set_target(const glm::vec3 &target)
{
    dir = glm::normalize(pos - target);
}

const glm::vec3 &Camera::get_front() const
{
    return dir;
}

void Camera::move(const glm::vec3 &delta)
{
    pos += dir * delta.z;
    glm::vec3 xdir = glm::normalize(glm::cross(up, dir));
    pos += xdir * delta.x;
    glm::vec3 ydir = glm::normalize(glm::cross(dir, xdir));
    pos += ydir * delta.y; 
}

void Camera::rotate(float rad, const glm::vec3 &axis)
{
    glm::mat4 matrix(1.0f);
    glm::vec3 xdir = glm::cross(up, dir);
    if (fabs(axis.x - 1) < 1e-6) {
        matrix = glm::rotate(matrix, rad, xdir);
        glm::vec3 temp_dir = matrix * glm::vec4(dir, 0.0f);
        if (glm::dot(glm::cross(up, temp_dir), xdir) > 0) {
            dir = temp_dir;
        }
    } else if (fabs(axis.y - 1) < 1e-6) {
        glm::vec3 ydir = glm::cross(dir, xdir);
        matrix = glm::rotate(matrix, rad, ydir);
        dir = matrix * glm::vec4(dir, 0.0f);
    } else if (fabs(axis.z - 1) < 1e-6) {
        matrix = glm::rotate(matrix, rad, dir);
        up = matrix * glm::vec4(up, 0.0f);
    }
}

glm::mat4 Camera::get_camera_matrix() const
{
    return glm::lookAt(pos, pos - dir, up);
}

void Camera::set_distance(float near, float far)
{
    this->near = near;
    this->far = far;
}

void Camera::get_distance(float &near, float &far) const
{
    near = this->near;
    far = this->far;
}

void Camera::set_size_ratio(float ratio)
{
    this->size_ratio = ratio;
}

float Camera::get_size_ratio() const
{
    return size_ratio;
}

glm::mat4 Camera::get_project_matrix() const
{
    return glm::perspective(fov, size_ratio, near, far);
}

void Camera::zoom(float times)
{
    fov = 2 * glm::atan(1 / times * glm::tan(fov / 2));
}

void Camera::set_fov(float rad)
{
    fov = rad;
}

float Camera::get_fov() const
{
    return fov;
}