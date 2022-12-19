#ifndef BILLBOARD_H
#define BILLBOARD_H

#include <initializer_list>
#include <string>
#include "glm.hpp"
#include "glad/glad.h"
#include "shader.h"

class BillBoard {
public:
    BillBoard();
    void init(const std::initializer_list<float> &pos, const std::string &texture);
    bool setProgram(const std::string &vs, const std::string &fs, const std::string &gs);
    void render(const glm::mat4 &VP, const std::initializer_list<float> &cameraPos);
private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_textureID;
    Shader m_program;
    GLuint m_pointCount;
};
#endif