#include "renderer.h"
#include "debug.h"

Renderer::Renderer() : m_lightCount{}
{
    if (!m_shader.generate_program({"..\\resource\\shader\\render\\vert.glsl"}, {"..\\resource\\shader\\render\\frag.glsl"})) {
        ERRINFO("Load render shader fail.");        
    }
}

void Renderer::addModel(std::shared_ptr<Model> pModel)
{
    if (!pModel) {
        return;
    }
    m_models.push_back(pModel);
}

void Renderer::removeModel(std::shared_ptr<Model> pModel)
{
    m_models.remove(pModel);
}

std::shared_ptr<Model> Renderer::getLastModel()
{
    return m_models.back();
}

void Renderer::addMesh(std::shared_ptr<Mesh> pMesh)
{
    if (!pMesh) {
        return;
    }
    m_meshes.push_back(pMesh);
}

void Renderer::removeMesh(std::shared_ptr<Mesh> pMesh)
{
    m_meshes.remove(pMesh);
}

void Renderer::setSkyBox(std::shared_ptr<SkyBox> pSkyBox)
{
    m_skybox = pSkyBox;
}

void Renderer::addLight(std::shared_ptr<Light> pLight)
{
    if (!pLight) {
        return;
    }
    auto type = pLight->type();
    if (m_lightCount[type] == k_maxLights) {
        ERRINFO("too many lights");
        return;
    }
    ++m_lightCount[type];
    m_lights.push_back(pLight);
}

void Renderer::removeLight(std::shared_ptr<Light> pLight)
{
    if (!pLight || find(m_lights.begin(), m_lights.end(), pLight) == m_lights.end()) {
        return;
    }
    auto type = pLight->type();
    --m_lightCount[type];
    m_lights.remove(pLight);
}

bool Renderer::render(const Camera &camera)
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!m_shader.valid()) {
        ERRINFO("Shader invalid");
        return false;
    }
    m_shader.use_program();
    if (!m_shader.set_uniform("mview", camera.get_camera_matrix())) {
        ERRINFO("Set uniform variable %s fail.", "mview");
        return false;
    }
    if (!m_shader.set_uniform("mproj", camera.get_project_matrix())) {
        ERRINFO("Set uniform variable %s fail.", "mproj");
        return false;
    }
    if (!m_shader.set_uniform("viewPos", camera.get_pos())) {
        ERRINFO("Set uniform variable %s fail.", "viewPos");
        return false;
    }
    if (!m_shader.set_uniform("pointLightCount", static_cast<unsigned>(m_lightCount[Light::k_pointLight]))) {
        ERRINFO("Set uniform variable %s fail.", "pointLightCount");
        return false;
    }
    if (!m_shader.set_uniform("dirLightCount", static_cast<unsigned>(m_lightCount[Light::k_dirLight]))) {
        ERRINFO("Set uniform variable %s fail.", "dirlightCount");
        return false;
    }
    if (!m_shader.set_uniform("spotLightCount", static_cast<unsigned>(m_lightCount[Light::k_spotLight]))) {
        ERRINFO("Set uniform variable %s fail.", "spotLightCount");
        return false;
    }

    int lightCount[Light::k_globalLight + 1]{0};
    for (auto l : m_lights) {
        auto type = l->type();
        if (!l->applyToShader(m_shader, lightCount[type])) {
            return false;
        }
        ++lightCount[type];
    }

    for (auto m : m_models) {
        m->draw(m_shader);
    }
    
    for (auto m : m_meshes) {
        m->draw(m_shader);
    }

    if (m_skybox) {
        m_skybox->draw(camera.get_camera_matrix(), camera.get_project_matrix());
    }
    return true;
}