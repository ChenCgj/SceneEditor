#ifndef SKYBOX_H
#define SKYBOX_H
#include <vector>
#include <string>
#include "glm.hpp"
#include "shader.h"
#include "mesh.h"

class SkyBox {
public:
    SkyBox(const std::vector<std::string> &imgs);
    ~SkyBox();
    bool draw(const glm::mat4 &mmview, const glm::mat4 &mproj);
private:
    Shader m_shader;
    Mesh m_mesh;
    GLuint m_texID;
};
#endif