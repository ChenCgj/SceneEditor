#ifndef MODEL_H
#define MODEL_H

#include "shader.h"
#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include "glad/glad.h"
#include "glm.hpp"

class Mesh;
struct Node;

class Model {
public:
    Model();
    bool loadModel(const std::string &filename);
    bool loadModel();
    void unload();
    void draw(Shader &program) const;
    void instanceDraw(Shader &program, int ninstance) const;
    void setTranslateMatrix(const glm::mat4 &matrix);
    void setBaseMatrix(const glm::mat4 &matrix);
    const glm::mat4 &getBaseMatrix() const;
    void getModelSize(double &w, double &h, double &d);
    void setInstanceAttrib(GLuint buffer, GLint attrib_pos, GLsizei attrib_size, GLenum type, GLuint size, void *offset, GLuint divide);
private:
    glm::mat4 modelMatrix;
    glm::mat4 baseMatrix;
    void drawNode(const Node *pnode, Shader &program) const;
    void instanceDrawNode(const Node *pnode, Shader &program, int ninstance) const;
    Node *mNode;
    std::unordered_map<std::string, GLuint> textureMap;
    double mSizeW, mSizeH, mSizeD;
};

#endif