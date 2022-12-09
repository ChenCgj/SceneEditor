#include "debug.h"
#include "glm.hpp"
#include "glad/glad.h"
#include "sceneEditorApp.h"
#include "mesh.h"
#include "gtc/matrix_transform.hpp"
#include "SDL.h"
#include "texture_manager.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::make_shared;
using glm::radians;
using glm::scale;
using glm::inverse;
using glm::normalize;

static void initOpengl();

SceneEditorApp::SceneEditorApp(int width, int height) : Application{"Scene Editor", width, height},
        m_skybox{{
            "..\\resource\\skyBox1\\sky_right.png",
            "..\\resource\\skyBox1\\sky_left.png",
            "..\\resource\\skyBox1\\sky_top.png",
            "..\\resource\\skyBox1\\sky_bottom.png",
            "..\\resource\\skyBox1\\sky_front.png",
            "..\\resource\\skyBox1\\sky_back.png",
        }}
{
    initOpengl();
    glViewport(0, 0, width, height);
    m_camera.set_size_ratio(width * 1.0 / height);
    m_camera.set_pos(vec3{0.0, 0.0, 5.0}, vec3{0, 0, 0});
    m_camera.set_distance(0.1, 1000);
    m_mesh = make_shared<Mesh>();

    if (!m_shader.generate_program({"..\\resource\\shader\\vert.glsl"}, {"..\\resource\\shader\\frag.glsl"})) {
        ERRINFO("generate shader fail.");
    }
    if (!m_model.loadModel("..\\resource\\models\\cornell_box.obj")) {
        ERRINFO("Load model %s%s fail.", SDL_GetBasePath(), "..\\resource\\model\\cornell_box.obj");
    }
    m_model.setBaseMatrix(scale(mat4(1.0), vec3(0.001, 0.001, 0.001)));
}

bool SceneEditorApp::render()
{
    glClearColor(0.3, 0.4, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glDepthMask(GL_FALSE);
    // glDepthMask(GL_TRUE);

    if (!m_shader.valid()) {
        ERRINFO("Shader invalid.");
        return false;
    }
    m_shader.use_program();
    if (!m_shader.set_uniform("mview", m_camera.get_camera_matrix())) {
        ERRINFO("Set uniform variable %s fail.", "mview");
        return false;
    }
    if (!m_shader.set_uniform("mproj", m_camera.get_project_matrix())) {
        ERRINFO("Set uniform variable %s fail.", "mproj");
        return false;
    }
    m_model.draw(m_shader);

    m_skybox.draw(m_camera.get_camera_matrix(), m_camera.get_project_matrix());
    return true;
}

void SceneEditorApp::dealButtonDown(const std::pair<int, int> &pos, MouseBtn button)
{
    if (button != MouseBtn::k_btnLeft || altKeyPressed()) {
        return;
    }
    auto size = getWindowSize();

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
    if (!leftButtonPressed() || !altKeyPressed()) {
        return;
    }
    m_camera.rotate(radians(rpos.first * 0.1), vec3{0, 1, 0});
    m_camera.rotate(radians(rpos.second * 0.1), vec3{1, 0, 0});
}

void SceneEditorApp::dealWheel(const std::pair<int, int> &pos, const std::pair<int, int> &scroll)
{
    float scale = 1.0;
    if (scroll.second > 0) {
        scale *= 1.1;
    } else {
        scale /= 1.1;
    }
    m_camera.zoom(scale);
}

void SceneEditorApp::dealKeyDown(SDL_Keycode key)
{
    const auto st = keyBoardStatu();
    vec3 move(0);
    if (st.count(SDLK_w)) {
        move[2] -= 0.2;
    }
    if (st.count(SDLK_s)) {
        move[2] += 0.2;
    }
    if (st.count(SDLK_a)) {
        move[0] -= 0.2;
    }
    if (st.count(SDLK_d)) {
        move[0] += 0.2;
    }
    if (st.count(SDLK_DOWN)) {
        move[1] -= 0.2;
    }
    if (st.count(SDLK_UP)) {
        move[1] += 0.2;
    }
    m_camera.move(move);
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
    m_shader.delete_program();
    m_mesh->unLoadData();
}

void initOpengl()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);
}