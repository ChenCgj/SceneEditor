#ifndef MODEL_H
#define MODEL_H

#include "shader.h"
#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include "glad/glad.h"
#include "glm.hpp"
#include "modelbuffer.h"

class Model {
public:
    Model();
    ~Model();
    void draw(Shader &program) const;
    void instanceDraw(Shader &program, int ninstance) const;
    void setTranslateMatrix(const glm::mat4 &matrix);
    void setBaseMatrix(const glm::mat4 &matrix);
    const glm::mat4 &getBaseMatrix() const;
    void SetAccessor(ModelBuffer * accessor_model);
    void setInstanceAttrib(GLuint buffer, GLint attrib_pos, GLsizei attrib_size, GLenum type, GLuint size, void *offset, GLuint divide);
private:
    glm::mat4 m_transformMatrix;
    glm::mat4 m_baseMatrix;
    void drawNode(const Node *pnode, Shader &program) const;
    void instanceDrawNode(const Node *pnode, Shader &program, int ninstance) const;
    ModelBuffer * accessor_model;
};

#endif