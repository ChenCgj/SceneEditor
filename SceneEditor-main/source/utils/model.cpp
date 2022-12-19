#include <iostream>
#include "debug.h"
#include "queue"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "mesh.h"
#include "model.h"
#include "modelbuffer.h"

using namespace std;

static std::pair<double, double> mergeRange(const std::pair<double, double> &r1, const pair<double, double> &r2)
{
    std::pair<double, double> ans;
    ans.first = std::min(r1.first, r2.first);
    ans.second = std::max(r1.second, r2.second);
    return ans;
}

Model::Model() : m_transformMatrix(1.0f), m_baseMatrix(1.0f) {}

Model::~Model()
{

}

void Model::SetAccessor(ModelBuffer * Ari)
{
    accessor_model = Ari;
    return;
}

void Model::draw(Shader &program) const
{
    // program.set_uniform("mmodel", modelMatrix * baseMatrix);
    drawNode(accessor_model->acceessorNode(), program);
}

void Model::instanceDraw(Shader &program, int ninstance) const
{
    // program.set_uniform("mmodel", modelMatrix * baseMatrix);
    instanceDrawNode(accessor_model->acceessorNode(), program, ninstance);
}

void Model::drawNode(const Node *pnode, Shader &program) const 
{
    if (!pnode) {
        return;
    }
    queue<const Node *> nodeq;
    nodeq.push(pnode);
    auto mmodel = m_transformMatrix * m_baseMatrix;
    while (!nodeq.empty()) {
        int size = nodeq.size();
        for (int i = 0; i < size; ++i) {
            const Node *n = nodeq.front();
            nodeq.pop();
            for (const auto pn : n->nextNodes) {
                nodeq.push(pn);
            }
            for (const auto pm : n->meshes) {
                pm->setTransformMatrix(mmodel);
                pm->draw(program);
            }
        }
    }
}

void Model::instanceDrawNode(const Node *pnode, Shader &program, int ninstance) const 
{
    queue<const Node *> nodeq;
    nodeq.push(pnode);
    auto mmodel = m_transformMatrix * m_baseMatrix;
    while (!nodeq.empty()) {
        int size = nodeq.size();
        for (int i = 0; i < size; ++i) {
            const Node *n = nodeq.front();
            nodeq.pop();
            for (const auto pn : n->nextNodes) {
                nodeq.push(pn);
            }
            for (const auto pm : n->meshes) {
                pm->setTransformMatrix(mmodel);
                pm->instanceDraw(program, ninstance);
            }
        }
    }
}

void Model::setTranslateMatrix(const glm::mat4 &matrix)
{
    m_transformMatrix = matrix;
}

void Model::setBaseMatrix(const glm::mat4 &matrix)
{
    m_baseMatrix = matrix;
}

const glm::mat4 &Model::getBaseMatrix() const
{
    return m_baseMatrix;
}