#include <vector>
#include "skyBox.h"
#include "debug.h"
#include "texture_manager.h"

using std::vector;
using glm::mat4;
using glm::mat3;

SkyBox::SkyBox() : m_texID(0)
{
    if (!m_shader.generate_program({"..\\resource\\shader\\skybox\\skybox_vert.glsl"}, {"..\\resource\\shader\\skybox\\skybox_frag.glsl"})) {
        ERRINFO("Generate skybox shader fail.");
        return;
    }
    vector<float> v = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    if (!m_mesh.loadData(v)) {
        ERRINFO("Load data fail.");
        m_shader.delete_program();
        return;
    }
    m_mesh.enableSetShader(false);
}

bool SkyBox::loadTexture(const std::vector<std::string> &imgs)
{
    if (m_texID == 0) {
        if ((m_texID = Texture_manager::instance()->load_cube_texture(imgs)) == 0) {
            ERRINFO("Load cube texture fail.");
            return false;
        }
    } else {
        if (!Texture_manager::instance()->reload_cube_texture(m_texID, imgs)) {
            ERRINFO("Load cube texture fail.");
            return false;
        }
    }
    return true;
}

bool SkyBox::draw(const glm::mat4 &mview, const glm::mat4 &mproj)
{
    if (!m_shader.valid()) {
        ERRINFO("skybox shader invalid");
        return false;
    }
    if (m_texID == 0) {
        ERRINFO("no skybox texture.");
        return false;
    }
    m_shader.use_program();
    if (!m_shader.set_uniform("mmodel", mat4(1))) {
        ERRINFO("Set skybox shader uniform variable %s fail.", "mmodel");
        return false;
    }
    if (!m_shader.set_uniform("mview", mat4(mat3(mview)))) {
        ERRINFO("Set skybox shader uniform variable %s fail.", "mview");
        return false;
    }
    if (!m_shader.set_uniform("mproj", mproj)) {
        ERRINFO("Set skybox shader uniform variable %s fail.", "mproj");
        return false;
    }
    if (!Texture_manager::instance()->bind_texture(m_shader, GL_TEXTURE_CUBE_MAP, "skybox", m_texID)) {
        ERRINFO("bind skybox texture fail.");
        return false;
    }
    glDepthFunc(GL_LEQUAL);
    m_mesh.draw(m_shader);
    glDepthFunc(GL_LESS);
    return true;
}

SkyBox::~SkyBox()
{
    m_shader.delete_program();
    m_mesh.unLoadData();
    Texture_manager::instance()->destroy_texture(m_texID);
}