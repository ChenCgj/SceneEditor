#ifndef MESH_H
#define MESH_H

#include <vector>
#include <unordered_map>
#include "glm.hpp"
#include "glad/glad.h"
#include "assimp/mesh.h"
#include "assimp/material.h"
#include "assimp/scene.h"
#include "shader.h"

class Mesh {
public:
    enum TextureType {
        k_texDiffuse, k_texSpecular, k_texAmbient, k_texEmissive, k_texHeight,
        k_texNormals, k_texShinness, k_texOpacity, k_texDisplacement, k_texLightmap,
        k_texReflection, k_texOther
    };
    Mesh();
    bool loadData(const aiMesh *mesh, const aiScene *scene, std::unordered_map<std::string, GLuint> &textureMap, const std::string &baseDir);
    bool loadData(const std::vector<float> &verties, const std::vector<float> &normal = {}, const std::vector<float> &color = {}, const std::vector<float> &uv = {},
                  const std::vector<float> &tangent = {}, const std::vector<float> &bitangent = {}, const std::vector<GLuint> &indies = {});
    void setBaseMatrix(const glm::mat4 &mat);
    glm::mat4 baseMatrix() const;
    void setTransformMatrix(const glm::mat4 &mat);
    void addTexture(TextureType type, GLuint texID);
    void unLoadData();
    void draw(Shader &program) const;
    void instanceDraw(Shader &program, int ninstance) const;
    void getMeshRange(std::pair<double, double> &rx, std::pair<double, double> &ry, std::pair<double, double> &rz);
    void setInstanceAttrib(GLuint buffer, GLint attrib_pos, GLsizei attrib_size, GLenum type, GLuint size, void *offset, GLuint divide);
    void enableSetShader(bool flag);
    ~Mesh();
private:
    bool setShaderParam(Shader &shader) const;
    static constexpr int k_texTypeCount = k_texOther + 1;
    static const std::string m_texName[k_texTypeCount];
    struct Vertex {
        std::vector<glm::vec3> m_pos;
        bool m_hasNormal;
        std::vector<glm::vec3> m_normal;
        bool m_hasUV;
        std::vector<glm::vec3> m_uv;
        bool m_hasColor;
        std::vector<glm::vec3> m_color;
        bool m_hasTangent;
        std::vector<glm::vec3> m_tangent;
        bool m_hasBitangent;
        std::vector<glm::vec3> m_bitangent;
    };
    bool bindData();
    void generateNormal();
    Vertex mVertexData;
    std::vector<GLuint> mIndices;
    std::vector<GLuint> mTex[k_texTypeCount];
    GLuint maxAttribPos;
    GLuint mVb, mVao, mVeb;
    std::pair<double, double> mRangeX, mRangeY, mRangeZ;
    glm::mat4 m_baseMatrix, m_transformMatrix;
    bool m_enableSetShader;
};

#endif