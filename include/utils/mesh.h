#ifndef MESH_H
#define MESH_H

#include <vector>
#include "unordered_map"
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
    bool loadData(const std::vector<float> &verties, const std::vector<float> &normal, const std::vector<float> &color, const std::vector<float> &uv,
                  const std::vector<float> &tangent, const std::vector<float> &bitangent, const std::vector<GLuint> &indies);
    void addTexture(TextureType type, GLuint texID);
    void unLoadData();
    void draw(Shader &program) const;
    void instanceDraw(Shader &program, int ninstance) const;
    void getMeshRange(std::pair<double, double> &rx, std::pair<double, double> &ry, std::pair<double, double> &rz);
    void setInstanceAttrib(GLuint buffer, GLint attrib_pos, GLsizei attrib_size, GLenum type, GLuint size, void *offset, GLuint divide);
    ~Mesh();
private:
    static constexpr int k_texTypeCount = k_texOther + 1;
    static const std::string m_texName[k_texTypeCount];
    struct Vertex {
        float pos[3];
        float normal[3];
        float color[3];
        float uv[2];
        float tangent[3];
        float bitangent[3];
    };
    bool bindData();
    std::vector<Vertex> mVertexData;
    std::vector<GLuint> mIndices;
    // std::vector<GLuint> mTextureDiffuseID, mTextureSpecularID, mTextureNormalID, mTextureHeightID, mTextureReflectionID;
    // std::vector<GLuint> mTextureColorID;
    std::vector<GLuint> mTex[k_texTypeCount];
    GLuint maxAttribPos;
    GLuint mVb, mVao, mVeb;
    std::pair<double, double> mRangeX, mRangeY, mRangeZ;
};

#endif