#ifndef SKYBOX_H
#define SKYBOX_H
#include <vector>
#include <string>
#include "glm.hpp"
#include "shader.h"
#include "mesh.h"

class SkyBox {
public:
    SkyBox();
    ~SkyBox();
    bool draw(const glm::mat4 &mmview, const glm::mat4 &mproj);
    bool loadTexture(const std::vector<std::string> &imgs);
private:
    Shader m_shader;
    Mesh m_mesh;
    GLuint m_texID;
};
#endif