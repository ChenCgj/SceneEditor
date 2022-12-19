#include <iostream>
#include "debug.h"
#include "queue"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "mesh.h"
#include "modelbuffer.h"

using namespace std;


static std::pair<double, double> mergeRange(const std::pair<double, double> &r1, const pair<double, double> &r2)
{
    std::pair<double, double> ans;
    ans.first = std::min(r1.first, r2.first);
    ans.second = std::max(r1.second, r2.second);
    return ans;
}

std::list<Mesh *>  ModelBuffer::AllMesh()
{
    std::list<Mesh *>  t;
    queue<const Node *> nodeq;
    nodeq.push(mNode);
    while (!nodeq.empty()) {
        int size = nodeq.size();
        for (int i = 0; i < size; ++i) {
            const Node *n = nodeq.front();
            nodeq.pop();
            for (const auto pn : n->nextNodes) {
                nodeq.push(pn);
            }
            for (const auto pm : n->meshes) {
                t.push_back(pm);
            }
        }
    }return t;
}

static void cleanNode(Node *node)
{
    if (!node) {
        return;
    }
    for (auto m : node->meshes) {
        m->unLoadData();
        delete m;
    }
    for (auto n : node->nextNodes) {
        cleanNode(n);
        delete n;
    }
}

static Node *getNode(const aiNode *node, const aiScene *scene, std::unordered_map<string, GLuint> &textureMap, const string &baseDir)
{
    Node *ret = new Node;
    ret->rangeFirstFlag = true;
    if (ret == nullptr) {
        ERRINFO("Load model error: not enought memory.");
        return ret;
    }
    ret->name = node->mName.C_Str();
    for (unsigned i = 0; i < node->mNumMeshes; ++i) {
        Mesh *mesh = new Mesh;
        if (mesh == nullptr) {
            ERRINFO("Load model error: not enought memory.");
            cleanNode(ret);
            return nullptr;
        }
        if (!mesh->loadData(scene->mMeshes[node->mMeshes[i]], scene, textureMap, baseDir)) {
            ERRINFO("load mesh data fail.");
            cleanNode(ret);
            mesh->unLoadData();
            delete mesh;
            return nullptr;
        }
        ret->meshes.push_back(mesh);
        if (ret->rangeFirstFlag) {
            mesh->getMeshRange(ret->rangeX, ret->rangeY, ret->rangeZ);
            ret->rangeFirstFlag = false;
        } else {
            pair<double, double> rx, ry, rz;
            mesh->getMeshRange(rx, ry, rz);
            ret->rangeX = mergeRange(ret->rangeX, rx);
            ret->rangeY = mergeRange(ret->rangeY, ry);
            ret->rangeZ = mergeRange(ret->rangeZ, rz);
        }
    }

    for (unsigned i = 0; i < node->mNumChildren; ++i) {
        Node *newNode = getNode(node->mChildren[i], scene, textureMap, baseDir);
        if (newNode == nullptr) {
            cleanNode(ret);
            return nullptr;
        }
        ret->nextNodes.push_back(newNode);
        if (ret->rangeFirstFlag) {
            ret->rangeX = newNode->rangeX;
            ret->rangeY = newNode->rangeY;
            ret->rangeZ = newNode->rangeZ;
            ret->rangeFirstFlag = false;
        } else {
            ret->rangeX = mergeRange(ret->rangeX, newNode->rangeX);
            ret->rangeY = mergeRange(ret->rangeY, newNode->rangeY);
            ret->rangeZ = mergeRange(ret->rangeZ, newNode->rangeZ);
        }
    }
    return ret;
}

ModelBuffer::ModelBuffer() : mNode(nullptr) {}

ModelBuffer::~ModelBuffer()
{
    unload();
}

bool ModelBuffer::loadModel()
{
    mNode = new Node;
    if (mNode == nullptr) {
        return false;
    }
    mNode->name = "default";
    Mesh *mesh = new Mesh();
    if (!mesh) {
        cleanNode(mNode);
        delete mNode;
        mNode = nullptr;
        return false;
    }
    mesh->loadData({}, {}, {}, {}, {}, {}, {});
    mNode->meshes.push_back(mesh);
    mSizeW = mSizeD = mSizeH = 1.0;
    return true;
}

Node* ModelBuffer::acceessorNode()
{
    return mNode;
}

bool ModelBuffer::loadModel(const std::string &filename)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        cerr << "Load Model fail: " << importer.GetErrorString() << endl;
        return false;
    }
    string baseDir = filename.substr(0, filename.find_last_of('\\') + 1);
    mNode = getNode(scene->mRootNode, scene, textureMap, baseDir);
    importer.FreeScene();
    if (mNode == nullptr) {
        return false;
    }
    mSizeW = mNode->rangeX.second - mNode->rangeX.first;
    mSizeH = mNode->rangeY.second - mNode->rangeY.first;
    mSizeD = mNode->rangeZ.second - mNode->rangeZ.first;
    return true;
}

void ModelBuffer::setInstanceAttrib(GLuint buffer, GLint attrib_pos, GLsizei attrib_size, GLenum type, GLuint size, void *offset, GLuint divide)
{
    queue<const Node *> nodeq;
    nodeq.push(mNode);
    while (!nodeq.empty()) {
        int qsize = nodeq.size();
        for (int i = 0; i < qsize; ++i) {
            const Node *n = nodeq.front();
            nodeq.pop();
            for (const auto pn : n->nextNodes) {
                nodeq.push(pn);
            }
            for (auto pm : n->meshes) {
                pm->setInstanceAttrib(buffer, attrib_pos, attrib_size, type, size, offset, divide);
            }
        }
    }
}

void ModelBuffer::getModelSize(double &w, double &h, double &d)
{
    w = mSizeW, h = mSizeH, d = mSizeD;
}

void ModelBuffer::unload()
{
    cleanNode(mNode);
    for (auto id : textureMap) {
        glDeleteTextures(1, &id.second);
    }
    textureMap.clear();
}