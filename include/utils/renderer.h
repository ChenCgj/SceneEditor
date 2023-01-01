#ifndef RENDERER_H
#define RENDERER_H
#include <memory>
#include <list>
#include "shader.h"
#include "model.h"
#include "mesh.h"
#include "skyBox.h"
#include "light.h"
#include "camera.h"

class Renderer {
public:
    Renderer();
    ~Renderer() = default;
    void addModel(std::shared_ptr<Model> pModel);
    void removeModel(std::shared_ptr<Model> pModel);
    std::shared_ptr<Model> getLastModel();
    void addMesh(std::shared_ptr<Mesh> pMesh);
    void removeMesh(std::shared_ptr<Mesh> pMesh);
    void addLight(std::shared_ptr<Light> pLight);
    void removeLight(std::shared_ptr<Light> pLight);
    void setSkyBox(std::shared_ptr<SkyBox> pSkyBox);
    bool render(const Camera &camera);
private:
    Shader m_shader;
    std::list<std::shared_ptr<Model>> m_models;
    std::list<std::shared_ptr<Mesh>> m_meshes;
    constexpr static int k_maxLights = 10;
    std::list<std::shared_ptr<Light>> m_lights;
    std::shared_ptr<SkyBox> m_skybox;
    int m_lightCount[Light::LightType::k_globalLight + 1];
};
#endif