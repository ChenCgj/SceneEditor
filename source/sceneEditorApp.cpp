#include <memory>
#include "debug.h"
#include "glm.hpp"
#include "glad/glad.h"
#include "mesh.h"
#include "gtc/matrix_transform.hpp"
#include "texture_manager.h"
#include "sceneEditorApp.h"
#include "SUI_utils.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::vector;
using std::make_shared;
using std::make_pair;
using glm::radians;
using glm::scale;
using glm::inverse;
using glm::normalize;

using namespace sui;
static void initOpengl();

SceneEditorApp::SceneEditorApp(int width, int height) : m_width(width), m_height(height)
{
    initOpengl();
    glViewport(0, 0, width, height);
    m_camera.set_size_ratio(width * 1.0 / height);
    m_camera.set_pos(vec3{0.0, 0.0, 5.0}, vec3{0, 0, 0});
    m_camera.set_distance(0.1, 1000);

    m_modelbuffer = make_shared<ModelBuffer>();
    m_model = make_shared<Model>();
    // if (!m_model->loadModel("..\\resource\\models\\mark\\mark.obj")) {
    // if (!m_model->loadModel("..\\resource\\models\\Humvee_models\\Humvee.obj")) {
    // if (!m_model->loadModel("..\\resource\\models\\mustang_gt\\Textures\\mustang_GT.obj")) {
    // if (!m_model->loadModel("..\\resource\\models\\face\\face.obj")) {
    // if (!m_model.loadModel("..\\resource\\models\\cornell_box.obj")) {
    if (!m_modelbuffer->loadModel("..\\resource\\models\\low_poly_tree\\Lowpoly_tree_sample.obj")) {
        ERRINFO("Load model %s fail.", "..\\resource\\model\\mark\\mark.obj");
    }

    m_model->SetAccessor(m_modelbuffer.get());
    m_skyBox = make_shared<SkyBox>();
    m_skyBox->loadTexture({
            "..\\resource\\skyBox1\\sky_right.png",
            "..\\resource\\skyBox1\\sky_left.png",
            "..\\resource\\skyBox1\\sky_top.png",
            "..\\resource\\skyBox1\\sky_bottom.png",
            "..\\resource\\skyBox1\\sky_front.png",
            "..\\resource\\skyBox1\\sky_back.png",
    });

    m_mesh = make_shared<Mesh>();
    vector<float> verties = {
        1, 1, 0,
        0, 1, 0,
        0, 0, 0,
        1, 0, 0
    };
    vector<float> color = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1,
        0, 1, 1
    };
    vector<float> uv = {
        1, 1, 0,
        0, 1, 0,
        0, 0, 0,
        1, 0, 0
    };
    m_mesh->loadData(verties, {}, color, uv, {}, {}, {0, 1, 2, 2, 3, 0});
    int id = Texture_manager::instance()->load_texture(GL_TEXTURE_2D, "..\\resource\\models\\face\\face.png");
    m_mesh->addTexture(Mesh::k_texDiffuse, id);

    m_light = std::make_shared<PointLight>(vec3(0, 0, 10), 1, 0.001, 0.01);
    m_light->setColor(glm::vec4(0), glm::vec4(1.0f, 0.6f, 0.0f, 1.0f), glm::vec4(1.0f, 0.6f, 0.0f, 1.0f));

    m_spotLight = std::make_shared<SpotLight>(glm::vec3(0, 0, 5), glm::vec3(0, 0, -5), 12.5, 17.5, 1, 0.01, 0.001);
    m_spotLight->setColor(glm::vec4(0), glm::vec4(0.8f, 0.8f, 0.0f, 1.0f), glm::vec4(0.8f, 0.8f, 0.0f, 1.0f));

    m_dirLight = std::make_shared<Dirlight>(glm::vec3(0, 0, -1));
    m_dirLight->setColor(glm::vec4(0.3f, 0.24f, 0.14f, 1.0f), glm::vec4(0.7f, 0.42f, 0.26f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    m_renderer.addMesh(m_mesh);
    m_renderer.addModel(m_model);
    m_renderer.addLight(m_light);
    m_renderer.addLight(m_spotLight);
    m_renderer.addLight(m_dirLight);
    m_renderer.setSkyBox(m_skyBox);
}

bool SceneEditorApp::render()
{
    m_renderer.render(m_camera);
    return true;
}

void SceneEditorApp::dealButtonDown(const std::pair<int, int> &pos, MouseBtn button)
{
    if (button != MouseBtn::k_btnLeft || is_alt_down()) {
        return;
    }
    auto size = make_pair(m_width, m_height);

    mat4 reset1{1.0};   // screen to ndc
    reset1[0][0] = 2.0 / size.first;
    reset1[1][1] = -2.0 / size.second;
    reset1[3][0] = -1;
    reset1[3][1] = 1;
    float near, far;
    m_camera.get_distance(near, far);
    reset1 *= near;     // reset divide

    mat4 reset2 = inverse(m_camera.get_camera_matrix()) * inverse(m_camera.get_project_matrix());
    vec4 posWorld = reset2 * reset1 * vec4{pos.first, pos.second, -1, 1};       // the world pos, actually it's the point at the near plane
    vec3 cameraPos = m_camera.get_pos();
    vec3 dir = vec3(posWorld) - cameraPos;
    dir = glm::normalize(dir);
    printf("the world pos is (%f, %f, %f), the camera pos is (%f, %f, %f), dir is (%f, %f, %f)\n",
           posWorld.x, posWorld.y, posWorld.z,
           cameraPos.x, cameraPos.y, cameraPos.z,
           dir.x, dir.y, dir.z); 
}

// void SceneEditorApp::dealButtonUp(const std::pair<int, int> &pos, MouseBtn button)
// {

// }

void SceneEditorApp::dealMouseMove(const std::pair<int, int> &pos, const std::pair<int, int> &rpos)
{
    if (!is_mouse_left_button_down() || !is_alt_down()) {
        return;
    }
    m_camera.rotate(radians(rpos.first * 0.1), vec3{0, 1, 0});
    m_camera.rotate(radians(rpos.second * 0.1), vec3{1, 0, 0});
    m_spotLight->setDir(-m_camera.get_front());
}
void SceneEditorApp::dealRotateModel(glm::vec3 axis, std::shared_ptr<Model> m_model)
{
    // std::shared_ptr<Model> m_model2 = make_shared<Model>();
    // m_model->SetAccessor(m_modelbuffer.get());
    glm::mat4 tran = m_model->getBaseMatrix();
    tran = glm::rotate(tran,glm::radians(5.0f),axis);
    m_model->setBaseMatrix(tran);
    // m_renderer.addModel(m_model);
}

void SceneEditorApp::dealMoveModel(glm::vec3 trans, std::shared_ptr<Model> m_model)
{
    // std::shared_ptr<Model> m_model2 = make_shared<Model>();
    // m_model->SetAccessor(m_modelbuffer.get());
    glm::mat4 tran = m_model->getBaseMatrix();
    tran = glm::translate(tran,trans);
    m_model->setBaseMatrix(tran);
    // m_renderer.addModel(m_model2);
}

void SceneEditorApp::dealScaleModel(glm::vec3 trans, std::shared_ptr<Model> m_model)
{
    // std::shared_ptr<Model> m_model2 = make_shared<Model>();
    // m_model->SetAccessor(m_modelbuffer.get());
    glm::mat4 tran = m_model->getBaseMatrix();
    tran = glm::scale(tran,trans);
    m_model->setBaseMatrix(tran);
    // m_renderer.addModel(m_model2);
}

void SceneEditorApp::dealRotateModel(glm::vec3 axis)
{
    // std::shared_ptr<Model> m_model2 = make_shared<Model>();
    // m_model->SetAccessor(m_modelbuffer.get());
    glm::mat4 tran = m_model->getBaseMatrix();
    tran = glm::rotate(tran,glm::radians(5.0f),axis);
    m_model->setBaseMatrix(tran);
    // m_renderer.addModel(m_model);
}

void SceneEditorApp::dealMoveModel(glm::vec3 trans)
{
    // std::shared_ptr<Model> m_model2 = make_shared<Model>();
    // m_model->SetAccessor(m_modelbuffer.get());
    glm::mat4 tran = m_model->getBaseMatrix();
    tran = glm::translate(tran,trans);
    m_model->setBaseMatrix(tran);
    // m_renderer.addModel(m_model2);
}

void SceneEditorApp::dealScaleModel(glm::vec3 trans)
{
    // std::shared_ptr<Model> m_model2 = make_shared<Model>();
    // m_model->SetAccessor(m_modelbuffer.get());
    glm::mat4 tran = m_model->getBaseMatrix();
    tran = glm::scale(tran,trans);
    m_model->setBaseMatrix(tran);
    // m_renderer.addModel(m_model2);
}

void SceneEditorApp::dealLoadModel(glm::vec3 trans)
{
    std::shared_ptr<Model> m_model = make_shared<Model>();
    // if (!m_model->loadModel("..\\resource\\models\\mark\\mark.obj")) {
    // if (!m_model->loadModel("..\\resource\\models\\Humvee_models\\Humvee.obj")) {
    // if (!m_model->loadModel("..\\resource\\models\\mustang_gt\\Textures\\mustang_GT.obj")) {
    // if (!m_model->loadModel("..\\resource\\models\\face\\face.obj")) {
    // if (!m_model.loadModel("..\\resource\\models\\cornell_box.obj")) {

    m_model->SetAccessor(m_modelbuffer.get());
    glm::mat4 tran = glm::mat4(1.0f);
    tran = glm::translate(tran,trans);
    m_model->setTranslateMatrix(tran);
    m_renderer.addModel(m_model);
}

void SceneEditorApp::dealWheel(const std::pair<int, int> &pos, float scroll)
{
    float scale = 1.0;
    if (scroll > 0) {
        scale *= 1.1;
    } else {
        scale /= 1.1;
    }
    m_camera.zoom(scale);
}

void SceneEditorApp::dealKeyDown(Key_code key)
{
    int num_key;
    const auto st = get_key_state(num_key);
    vec3 move(0);
    if (st[key_w]) {
        move[2] -= 0.2;
    }
    if (st[key_s]) {
        move[2] += 0.2;
    }
    if (st[key_a]) {
        move[0] -= 0.2;
    }
    if (st[key_d]) {
        move[0] += 0.2;
    }
    // if (st[key_]) {
    //     move[1] -= 0.2;
    // }
    // if (st.count(SDLK_UP)) {
    //     move[1] += 0.2;
    // }
    m_camera.move(move);
    m_spotLight->setPos(m_camera.get_pos());
    m_spotLight->setDir(-m_camera.get_front());
}

// void SceneEditorApp::dealKeyUp(SDL_Keycode key)
// {

// }

void SceneEditorApp::dealWinSizeChange(const std::pair<int, int> &size)
{
    m_camera.set_size_ratio(size.first * 1.0 / size.second);
    glViewport(0, 0, size.first, size.second);
}

// void SceneEditorApp::dealOther(const SDL_Event *pe)
// {

// }

SceneEditorApp::~SceneEditorApp()
{

}

void initOpengl()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);
}