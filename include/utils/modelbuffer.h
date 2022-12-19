#ifndef MODELBUFFER_H
#define MODELBUFFER_H

#include "shader.h"
#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include "glad/glad.h"
#include "glm.hpp"

class Mesh;
struct Node;

struct Node {
    std::string name;
    std::vector<Mesh *> meshes;
    std::list<Node *> nextNodes;
    std::pair<double, double> rangeX, rangeY, rangeZ;
    bool rangeFirstFlag;
};


class ModelBuffer {
public:
    ModelBuffer();
    ~ModelBuffer();
    bool loadModel(const std::string &filename);
    bool loadModel();
    void unload();
    Node *acceessorNode();
    void getModelSize(double &w, double &h, double &d);
    void setInstanceAttrib(GLuint buffer, GLint attrib_pos, GLsizei attrib_size, GLenum type, GLuint size, void *offset, GLuint divide);
    std::list<Mesh *> AllMesh();

private:
    Node *mNode;
    std::unordered_map<std::string, GLuint> textureMap;
    double mSizeW, mSizeH, mSizeD;
};

#endif