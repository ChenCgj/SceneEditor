#include <iostream>
#include <algorithm>
#include "debug.h"
#include "mesh.h"
#include "texture_manager.h"
#define TEX_MGR Texture_manager::instance()

using namespace std;

static void saveRange(std::pair<double, double> &p, double value)
{
    p.first = std::min(p.first, value);
    p.second = std::max(p.first, value);
}

const std::string Mesh::m_texName[k_texTypeCount] = {
    "texDiff", "texSpec", "texAmbi", "texEmis",
    "texHeig", "texNorm", "texShin", "texOpac",
    "texDisp", "texLigh", "texRefl", "texOther"
};

static aiTextureType types[] = {
    aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_AMBIENT, aiTextureType_EMISSIVE,
    aiTextureType_HEIGHT, aiTextureType_NORMALS, aiTextureType_SHININESS, aiTextureType_OPACITY,
    aiTextureType_DISPLACEMENT, aiTextureType_LIGHTMAP, aiTextureType_REFLECTION, aiTextureType_UNKNOWN
};

Mesh::Mesh() : mVb(0), mVao(0), mVeb(0), m_baseMatrix(1.0), m_transformMatrix(1.0), m_enableSetShader(true)
{
    glGenBuffers(1, &mVb);
    glGenBuffers(1, &mVeb);
    if (mVb == 0 || mVeb == 0) {
        if (mVb) {
            glDeleteBuffers(1, &mVb);
        }
        if (mVeb) {
            glDeleteBuffers(1, &mVeb);
        }
        ERRINFO("Create buffer fail.");
        return;
    }
    glGenVertexArrays(1, &mVao);
    if (mVao == 0) {
        glDeleteBuffers(1, &mVb);
        glDeleteBuffers(1, &mVeb);
        ERRINFO("Create verties array fail.");
        return;
    }
}

Mesh::~Mesh()
{
    unLoadData();
    if (mVao) {
        glDeleteVertexArrays(1, &mVao);
        mVao = 0;
    }
    if (mVb) {
        glDeleteBuffers(1, &mVb);
        mVb = 0;
    }
    if (mVeb) {
        glDeleteBuffers(1, &mVeb);
        mVeb = 0;
    }
}

GLuint getTextureID(const char *str, unordered_map<std::string, GLuint> &textureMap, const string &baseDir)
{
    if (textureMap.find(str) != textureMap.end()) {
        return textureMap[str];
    } else {
        GLuint id = TEX_MGR->load_texture(GL_TEXTURE_2D, baseDir + str);
        if (id == 0) {
            return 0;
        }
        textureMap[str] = id;
        return id;
    }
}

void loadTexture(aiTextureType type, const aiMaterial *material, vector<GLuint> &container, unordered_map<std::string, GLuint> &textureMap, const string &baseDir)
{
    unsigned count = material->GetTextureCount(type);
    for (unsigned i = 0; i < count; ++i) {
        aiString str;
        material->GetTexture(type, i, &str);
        GLuint id = getTextureID(str.C_Str(), textureMap, baseDir);
        container.push_back(id);
    }
}

void Mesh::generateNormal()
{
    auto nVert = mVertexData.m_pos.size();
    mVertexData.m_hasNormal = true;
    mVertexData.m_normal = std::vector<glm::vec3>(nVert, glm::vec3{0, 0, 0});
    std::vector<int> faceCount(nVert, 0);
    for (unsigned int i = 0; i + 2 < mIndices.size(); i += 3) {
        GLuint index[] = {mIndices[i], mIndices[i + 1], mIndices[i + 2]};
        // calcuate the normal
        glm::vec3 fnormal = glm::cross(mVertexData.m_pos[index[1]] - mVertexData.m_pos[index[0]], mVertexData.m_pos[index[2]] - mVertexData.m_pos[index[0]]);
        fnormal = glm::normalize(fnormal);
        for (auto j : index) {
            mVertexData.m_normal[j] = (mVertexData.m_normal[j] * static_cast<float>(faceCount[j]) + fnormal) / static_cast<float>(faceCount[j] + 1);
            faceCount[j]++;
        }
    }
    mVertexData.m_hasNormal = true;
}

bool Mesh::loadData(const vector<float> &verties, const vector<float> &normal, const vector<float> &color, const vector<float> &uv,
                    const vector<float> &tangent, const vector<float> &bitangent, const vector<GLuint> &indies)
{
    unLoadData();
    int nVert = verties.size() * sizeof(verties[0]) / sizeof(mVertexData.m_pos[0]);
    int nNorm = normal.size() * sizeof(verties[0]) / sizeof(mVertexData.m_normal[0]);
    int nColor = color.size() * sizeof(color[0]) / sizeof(mVertexData.m_color[0]);
    int nUv = uv.size() * sizeof(uv[0]) / sizeof(mVertexData.m_uv[0]);
    int nTangent = tangent.size() * sizeof(tangent[0]) / sizeof(mVertexData.m_tangent[0]);
    int nBitangent = bitangent.size() * sizeof(bitangent[0]) / sizeof(mVertexData.m_bitangent[0]);
    mVertexData.m_pos.resize(nVert);
    memcpy(&mVertexData.m_pos[0], &verties[0], nVert * sizeof(mVertexData.m_pos[0]));
    if (nNorm == nVert) {
        mVertexData.m_hasNormal = true;
        mVertexData.m_normal.resize(nVert);
        memcpy(&mVertexData.m_normal[0], &normal[0], nVert * sizeof(mVertexData.m_normal[0]));
    }
    if (nColor == nVert) {
        mVertexData.m_hasColor = true;
        mVertexData.m_color.resize(nVert);
        memcpy(&mVertexData.m_color[0], &color[0], nVert * sizeof(mVertexData.m_color[0]));
    }
    if (nUv == nVert) {
        mVertexData.m_hasUV = true;
        mVertexData.m_uv.resize(nVert);
        memcpy(&mVertexData.m_uv[0], &uv[0], nVert * sizeof(mVertexData.m_uv[0]));
    }
    mVertexData.m_hasTangent = mVertexData.m_hasBitangent = false;
    if (indies.size() == 0) {
        mIndices.resize(nVert);
        int index = 0;
        std::generate(mIndices.begin(), mIndices.end(), [&](){return index++;});
    } else {
        mIndices = indies;
    }
    if (!mVertexData.m_hasNormal) {
        generateNormal();
    }
    return bindData();
}

bool Mesh::loadData(const aiMesh *mesh, const aiScene *scene, unordered_map<std::string, GLuint> &textureMap, const string &baseDir)
{
    unLoadData();
    aiColor3D color(1.0f, 1.0f, 1.0f);
    if (mesh->mMaterialIndex >= 0) {
        const aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        for (int i = 0; i < k_texTypeCount; ++i) {
            loadTexture(types[i], material, mTex[i], textureMap, baseDir);
        }
    }
    mVertexData.m_pos.resize(mesh->mNumVertices);
    memcpy(&mVertexData.m_pos[0], mesh->mVertices, mesh->mNumVertices * sizeof(mVertexData.m_pos[0]));
    mVertexData.m_hasColor = true;
    mVertexData.m_color = vector<glm::vec3>(mesh->mNumVertices, glm::vec3(color[0], color[1], color[2]));
    if (mesh->HasNormals()) {
        mVertexData.m_hasNormal = true;
        mVertexData.m_normal.resize(mesh->mNumVertices);
        memcpy(&mVertexData.m_normal[0], mesh->mNormals, mesh->mNumVertices * sizeof(mVertexData.m_normal[0]));
    } else {
        mVertexData.m_hasNormal = false;
    }
    if (mesh->HasTextureCoords(0)) {
        mVertexData.m_hasUV = true;
        mVertexData.m_uv.resize(mesh->mNumVertices);
        memcpy(&mVertexData.m_uv[0], mesh->mTextureCoords[0], mesh->mNumVertices * sizeof(mVertexData.m_uv[0]));
    } else {
        mVertexData.m_hasUV = false;
    }
    mVertexData.m_hasTangent = mVertexData.m_hasBitangent = false;

    for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace &f = mesh->mFaces[i];
        for (unsigned j = 0; j < f.mNumIndices; ++j) {
            mIndices.push_back(f.mIndices[j]);
        }
    }
    if (!mVertexData.m_hasNormal) {
        generateNormal();
    }
    return bindData();
}

void Mesh::setBaseMatrix(const glm::mat4 &mat)
{
    m_baseMatrix = mat;
}

glm::mat4 Mesh::baseMatrix() const
{
    return m_baseMatrix;
}

void Mesh::setTransformMatrix(const glm::mat4 &mat)
{
    m_transformMatrix = mat;
}

void Mesh::addTexture(TextureType type, GLuint texID)
{
    mTex[type].push_back(texID);
}

bool Mesh::bindData()
{
    if (!mVb || !mVeb || !mVao) {
        return false;
    }
    int nVert = mVertexData.m_pos.size();
    unsigned int totalSize = sizeof(mVertexData.m_pos[0]) * nVert +
                             sizeof(mVertexData.m_normal[0]) * nVert * mVertexData.m_hasNormal +
                             sizeof(mVertexData.m_color[0]) * nVert * mVertexData.m_hasColor +
                             sizeof(mVertexData.m_uv[0]) * nVert * mVertexData.m_hasUV +
                             sizeof(mVertexData.m_tangent[0]) * nVert * mVertexData.m_hasTangent +
                             sizeof(mVertexData.m_bitangent[0]) * nVert * mVertexData.m_hasBitangent;

    unsigned int offset = 0;
    GLuint attribPos = 0;

    glBindBuffer(GL_ARRAY_BUFFER, mVb);
    glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);
    glBindVertexArray(mVao);

#define LOAD_ATTRIB(cond, firstEle) do {\
        if ((cond)) {\
            glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(firstEle) * nVert, &firstEle);\
            glVertexAttribPointer(attribPos, sizeof(firstEle) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(firstEle), reinterpret_cast<void *>(offset));\
            glEnableVertexAttribArray(attribPos);\
            offset += sizeof(firstEle) * nVert;\
        }\
        ++attribPos;\
    } while(0)

    LOAD_ATTRIB(true, mVertexData.m_pos[0]);
    LOAD_ATTRIB(mVertexData.m_hasNormal, mVertexData.m_normal[0]);
    LOAD_ATTRIB(mVertexData.m_hasColor, mVertexData.m_color[0]);
    LOAD_ATTRIB(mVertexData.m_hasUV, mVertexData.m_uv[0]);
    LOAD_ATTRIB(mVertexData.m_hasTangent, mVertexData.m_tangent[0]);
    LOAD_ATTRIB(mVertexData.m_hasBitangent, mVertexData.m_bitangent[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVeb);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mIndices.size(), &mIndices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    maxAttribPos = 5;
    return true;
}

void Mesh::unLoadData()
{
    mVertexData.m_pos.clear();
    mVertexData.m_normal.clear();
    mVertexData.m_color.clear();
    mVertexData.m_uv.clear();
    mVertexData.m_tangent.clear();
    mVertexData.m_bitangent.clear();
    mVertexData.m_hasNormal = false;
    mVertexData.m_hasColor = false;
    mVertexData.m_hasUV = false;
    mVertexData.m_hasTangent = false;
    mVertexData.m_hasBitangent = false;
    mIndices.clear();
    for (int i = 0; i < k_texTypeCount; ++i) {
        for (auto id : mTex[i]) {
            Texture_manager::instance()->destroy_texture(id);
        }
        mTex[i].clear();
    }
}

void Mesh::enableSetShader(bool flag)
{
    m_enableSetShader = flag;
}

bool Mesh::setShaderParam(Shader &shader) const
{
    if (!m_enableSetShader) {
        return true;
    }
    bool ret = shader.set_uniform("mmodel", m_transformMatrix * m_baseMatrix) &&
               shader.set_uniform("colorValid", mVertexData.m_color.size() != 0) &&
               shader.set_uniform("diffValid", mTex[k_texDiffuse].size() != 0) &&
               shader.set_uniform("specValid", mTex[k_texSpecular].size() != 0) &&
               shader.set_uniform("normValid", mTex[k_texNormals].size() != 0) &&
            //    shader.set_uniform("heigValid", mTex[k_texHeight].size() != 0) &&
               true;
    return ret;
}

void Mesh::draw(Shader &program) const
{
    TEX_MGR->clean();
    for (int i = 0; i < k_texTypeCount; ++i) {
        for (vector<GLuint>::size_type j = 0; j < mTex[i].size(); ++j) {
            TEX_MGR->bind_texture(program, GL_TEXTURE_2D, m_texName[i] + to_string(j + 1), mTex[i][j]);
        }
    }
    if (!setShaderParam(program)) {
        glActiveTexture(0);
        ERRINFO("Set shader parameters fail.");
        return;
    }
    glBindVertexArray(mVao);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glActiveTexture(0);
}

void Mesh::instanceDraw(Shader &program, int ninstance) const
{
    TEX_MGR->clean();
    for (int i = 0; i < k_texTypeCount; ++i) {
        for (vector<GLuint>::size_type j = 0; j < mTex[i].size(); ++j) {
            TEX_MGR->bind_texture(program, GL_TEXTURE_2D, m_texName[i] + to_string(j + 1), mTex[i][j]);
        }
    }
    if (!setShaderParam(program)) {
        glActiveTexture(0);
        ERRINFO("Set shader prarameters fail.");
        return;
    }
    glBindVertexArray(mVao);
    glDrawElementsInstanced(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr, ninstance);
    glBindVertexArray(0);
    glActiveTexture(0);
}

void Mesh::getMeshRange(std::pair<double, double> &rx, std::pair<double, double> &ry, std::pair<double, double> &rz)
{
    rx = mRangeX;
    ry = mRangeY;
    rz = mRangeZ;
}

void Mesh::setInstanceAttrib(GLuint buffer, GLint attrib_pos, GLsizei attrib_size, GLenum type, GLuint size, void *offset, GLuint divide)
{
    glBindVertexArray(mVao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(attrib_pos);
    glVertexAttribPointer(attrib_pos, attrib_size, type, GL_FALSE, size, offset);
    glVertexAttribDivisor(attrib_pos, divide);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
