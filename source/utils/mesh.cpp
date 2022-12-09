#include <iostream>
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

Mesh::Mesh() : mVb(0), mVao(0), mVeb(0)
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

bool Mesh::loadData(const vector<float> &verties, const vector<float> &normal, const vector<float> &color, const vector<float> &uv,
                    const vector<float> &tangent, const vector<float> &bitangent, const vector<GLuint> &indies)
{
    unLoadData();
    int sv = sizeof(Vertex::pos) / sizeof(float), sn = sizeof(Vertex::normal) / sizeof(float), sc = sizeof(Vertex::color) / sizeof(float);
    int suv = sizeof(Vertex::uv) / sizeof(float), st = sizeof(Vertex::tangent) / sizeof(float), sb = sizeof(Vertex::bitangent) / sizeof(float);
    int vertexSize = verties.size() / sv;
    int sizen = normal.size() / sn;
    int sizec = color.size() / sc;
    int sizeuv = uv.size() / suv;
    int sizet = tangent.size() / st;
    int sizeb = bitangent.size() / sb;
    if ((sizen != vertexSize && sizen) || (sizec != vertexSize && sizec) ||
         (sizeuv != vertexSize && sizeuv) || (sizet != vertexSize && sizet) || (sizeb != vertexSize && sizeb)) {
        return false;
    }
    mVertexData.resize(vertexSize);
    Vertex zeroMem = {{}, {}, {}, {}, {}, {}};
    for (int i = 0; i < vertexSize; ++i) {
        memcpy(mVertexData[i].pos, &verties[i * sv], sv * sizeof(float));
        memcpy(mVertexData[i].normal, i < sizen ? &normal[i * sn] : zeroMem.normal, sn * sizeof(float));
        memcpy(mVertexData[i].color, i < sizec ? &color[i * sc] : zeroMem.color, sc * sizeof(float));
        memcpy(mVertexData[i].uv, i < sizeuv ? &uv[i * suv] : zeroMem.uv, suv * sizeof(float));
        memcpy(mVertexData[i].tangent, i < sizet ? &tangent[i * st] : zeroMem.tangent, st * sizeof(float));
        memcpy(mVertexData[i].bitangent, i < sizeb ? &bitangent[i * sb] : zeroMem.bitangent, sb * sizeof(float));
    }
    mIndices = indies;
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
    mVertexData.resize(mesh->mNumVertices);
    for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
        mVertexData[i].pos[0] = mesh->mVertices[i].x;
        mVertexData[i].pos[1] = mesh->mVertices[i].y;
        mVertexData[i].pos[2] = mesh->mVertices[i].z;
        if (i == 0) {
            mRangeX = make_pair(mVertexData[i].pos[0], mVertexData[i].pos[0]);
            mRangeY = make_pair(mVertexData[i].pos[1], mVertexData[i].pos[1]);
            mRangeZ = make_pair(mVertexData[i].pos[2], mVertexData[i].pos[2]);
        } else {
            saveRange(mRangeX, mVertexData[i].pos[0]);
            saveRange(mRangeY, mVertexData[i].pos[1]);
            saveRange(mRangeZ, mVertexData[i].pos[2]);
        }

        if (mesh->HasNormals()) {
            mVertexData[i].normal[0] = mesh->mNormals[i].x;
            mVertexData[i].normal[1] = mesh->mNormals[i].y;
            mVertexData[i].normal[2] = mesh->mNormals[i].z;
        } else {
            mVertexData[i].normal[0] = 0.0f;
            mVertexData[i].normal[1] = 0.0f;
            mVertexData[i].normal[2] = 0.0f;
        }

        mVertexData[i].color[0] = color[0];
        mVertexData[i].color[1] = color[1];
        mVertexData[i].color[2] = color[2];
        if (mesh->HasTextureCoords(0)) {
            mVertexData[i].uv[0] = mesh->mTextureCoords[0][i].x;
            mVertexData[i].uv[1] = mesh->mTextureCoords[0][i].y;
            // mVertexData[i].tangent[0] = mesh->mTangents[i].x;
            // mVertexData[i].tangent[1] = mesh->mTangents[i].y;
            // mVertexData[i].tangent[2] = mesh->mTangents[i].z;
            // mVertexData[i].bitangent[0] = mesh->mBitangents[i].x;
            // mVertexData[i].bitangent[1] = mesh->mBitangents[i].y;
            // mVertexData[i].bitangent[2] = mesh->mBitangents[i].z;
        } else {
            mVertexData[i].uv[0] = 0.0f;
            mVertexData[i].uv[1] = 0.0f;
            mVertexData[i].tangent[0] = 0.0f;
            mVertexData[i].tangent[1] = 0.0f;
            mVertexData[i].tangent[2] = 0.0f;
            mVertexData[i].bitangent[0] = 0.0f;
            mVertexData[i].bitangent[1] = 0.0f;
            mVertexData[i].bitangent[2] = 0.0f;
        }
    }

    for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace &f = mesh->mFaces[i];
        for (unsigned j = 0; j < f.mNumIndices; ++j) {
            mIndices.push_back(f.mIndices[j]);
        }
    }
    return bindData();
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
    glBindBuffer(GL_ARRAY_BUFFER, mVb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertexData.size(), &mVertexData[0], GL_STATIC_DRAW);

    glBindVertexArray(mVao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, color)));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, uv)));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, tangent)));
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, bitangent)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
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
    mVertexData.clear();
    mIndices.clear();
    for (int i = 0; i < k_texTypeCount; ++i) {
        mTex[i].clear();
    }
}

void Mesh::draw(Shader &program) const
{
    TEX_MGR->clean();
    for (int i = 0; i < k_texTypeCount; ++i) {
        for (vector<GLuint>::size_type j = 0; j < mTex[i].size(); ++j) {
            TEX_MGR->bind_texture(program, GL_TEXTURE_2D, m_texName[i] + to_string(j + 1), mTex[i][j]);
        }
    }
    glBindVertexArray(mVao);
    if (mIndices.size()) {
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, mVertexData.size());
    }
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
