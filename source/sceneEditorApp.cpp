#include <memory>
#include "debug.h"
#include "glm.hpp"
#include "glad/glad.h"
#include "mesh.h"
#include "gtc/matrix_transform.hpp"
#include "texture_manager.h"
#include "sceneEditorApp.h"
#include "SUI_utils.h"
#include <map>
#include <cmath>

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
std::map<std::pair<float, float>, float> geth;
using namespace sui;
static void initOpengl();
int h, w, pitch;


SceneEditorApp::SceneEditorApp(int width, int height) : m_width(width), m_height(height), status(false), m_modelBufferIndex(0), m_skyBoxIndex(0)
{
    initOpengl();
    glViewport(0, 0, width, height);
    m_camera.set_size_ratio(width * 1.0 / height);
    m_camera.set_pos(vec3{9.0, 3.0, 5.0}, vec3{0, 0, 0});
    m_camera.set_distance(0.1, 1000);

    std::shared_ptr<SkyBox> m_skyBox1 = make_shared<SkyBox>();
    std::shared_ptr<SkyBox> m_skyBox2 = make_shared<SkyBox>();
    m_skyBox1->loadTexture({
            "..\\resource\\skyBox2\\sky_right.jpg",
            "..\\resource\\skyBox2\\sky_left.jpg",
            "..\\resource\\skyBox2\\sky_top.jpg",
            "..\\resource\\skyBox2\\sky_bottom.jpg",
            "..\\resource\\skyBox2\\sky_front.jpg",
            "..\\resource\\skyBox2\\sky_back.jpg",
    });
    m_skyBoxList.push_back(m_skyBox1);
        m_skyBox2->loadTexture({
            "..\\resource\\skyBox1\\sky_right.png",
            "..\\resource\\skyBox1\\sky_left.png",
            "..\\resource\\skyBox1\\sky_top.png",
            "..\\resource\\skyBox1\\sky_bottom.png",
            "..\\resource\\skyBox1\\sky_front.png",
            "..\\resource\\skyBox1\\sky_back.png",
    });
    m_skyBoxList.push_back(m_skyBox2);
    m_renderer.setSkyBox(m_skyBoxList[0]);

    m_light = std::make_shared<PointLight>(vec3(0, 0, 10), 1, 0.001, 0.01);
    m_light->setColor(glm::vec4(0), glm::vec4(1.0f, 0.6f, 0.0f, 1.0f), glm::vec4(1.0f, 0.6f, 0.0f, 1.0f));

    m_spotLight = std::make_shared<SpotLight>(glm::vec3(0, 0, 5), glm::vec3(0, 0, -5), 12.5, 17.5, 1, 0.01, 0.001);
    m_spotLight->setColor(glm::vec4(0), glm::vec4(0.8f, 0.8f, 0.0f, 1.0f), glm::vec4(0.8f, 0.8f, 0.0f, 1.0f));

    m_dirLight = std::make_shared<Dirlight>(glm::vec3(0, 0, -1));
    m_dirLight->setColor(glm::vec4(0.3f, 0.24f, 0.14f, 1.0f), glm::vec4(0.7f, 0.42f, 0.26f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    m_renderer.addLight(m_light);
    m_renderer.addLight(m_spotLight);
    m_renderer.addLight(m_dirLight);

    vector<float> verties, color, uv;
    m_mesh = make_shared<Mesh>();

    extern char *load_image_data(const std::string &image, int &width, int &height, int &pitch);
    char *pict = load_image_data("..\\resource\\place\\place1.jpg", h, w, pitch);
    for (int i = 0; i < h; i += 1) {
        int k = 0;
        while (k < w) {
            verties.push_back(i * 10);
            verties.push_back((pict[i * pitch + k * 3] + pict[i * pitch + k * 3 + 1] + pict[i * pitch + k * 3 + 2]) / 60.0f);
            verties.push_back(k * 10);
            geth[{i * 0.1f, k * 0.1f}] = (pict[i * pitch + k * 3] + pict[i * pitch + k * 3 + 1] + pict[i * pitch + k * 3 + 2]) / 60.0f;
            for (int j = 0; j < 3; j++) {
                color.push_back(0.5f);
            }
            uv.push_back(i * 1.0 / h);
            uv.push_back(k * 1.0 / w);
            uv.push_back(0);
            k += 1;
        }
    }

    vector<GLuint> seq;
    for (int i = 0; i < h; i += 1) {
        for (int j = 0; j < w; j += 1) {
            if (j + 1 < w && i + 1 < h) {
                seq.push_back(j + w * (i ));
                seq.push_back(j+ w + w * (i ));
                seq.push_back(j+ w + 1+ w * (i ));
                seq.push_back(j+ w * (i));
                seq.push_back(j + w + 1+ w * (i ));
                seq.push_back(j + 1+ w * (i ));
            }
        }
    }
    m_mesh->loadData(verties, {}, color, uv, {}, {}, seq);
    m_mesh->setBaseMatrix(glm::scale(glm::mat4(1.0), vec3(0.1)));
    delete pict;
    int id = Texture_manager::instance()->load_texture(GL_TEXTURE_2D, "..\\resource\\models\\tet\\4.jpg");
    m_mesh->addTexture(Mesh::k_texDiffuse, id);
    m_renderer.addMesh(m_mesh);

    m_mMove = m_mScale = m_mRotate = m_mPos = mat4(1.0);
}

bool SceneEditorApp::render()
{
    m_renderer.render(m_camera);
    return true;
}

bool SceneEditorApp::getStatus()
{
    return status;
}

void SceneEditorApp::setStatus()
{
    status = !status;
}

void SceneEditorApp::dealButtonDown(const std::pair<int, int> &pos, MouseBtn button)
{
    if ((button != MouseBtn::k_btnLeft && button != MouseBtn::k_btnRight) || is_alt_down()) {
        return;
    }
    if(button == MouseBtn::k_btnRight)
    {
        m_renderer.removeModel(m_currModel);
        m_currModel = m_renderer.getLastModel();
        return ;
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
    // if(status)
    // {
    //     dealLoadModel(glm::vec3(posWorld.x,posWorld.y,posWorld.z));
    // }

    // m_mesh; // m_mesh 是要处理的元
    // std::vector<glm::vec3> temp = m_mesh.getpos();
    
    // // 先与法线计算求得 t
    // // 然后判断点是否在平面内

    // auto find = [&](const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &p) {
    //     // 判断点是否在三角形内
    //     if (glm::dot(glm::cross(a - b, a - c), glm::cross(a - b, a - p)) > 0 && glm::dot(glm::cross(b - c, b - a), glm::cross(b - c, b - p)) > 0 
    //             && glm::dot(glm::cross(c - b, c - a), glm::cross(c - b, c - p)) > 0) {
    //                 return true;
    //             }
    //     return false; 
    // };

    // int n = temp.size() / 3;
    // glm::vec3 intersection;
    // for (int i = 0; i < n; i++) {
    //     glm::vec3 a = temp[i * 3];
    //     glm::vec3 b = temp[i * 3 + 1];
    //     glm::vec3 c = temp[i * 3 + 2];

    //     glm::vec3 p1 = a - b;
    //     glm::vec3 p2 = b - c;
    //     glm::vec3 norm = -glm::cross(p1, p2);

    //     // norm.x = p1.y * p2.z - p1.z * p2.y;
    //     // norm.y = p1.z * p2.x - p1.x * p2.z;
    //     // norm.z = p1.x * p2.y - p1.y * p2.x;

    //     // norm = glm::normalize(norm);

    //     int t = glm::dot((a - cameraPos), norm) / glm::dot(dir, norm);
    //     glm::vec3 p = cameraPos + glm::vec3(dir.x * t, dir.y * t, dir.z * t);
    //     if (find(a, b, c, p)) {
    //         intersection = p;
    //         break;
    //     }
    // }

    glm::vec3 step = {dir.x * 8, dir.y * 8, dir.z * 8}; // 还差一个 blocksize
    glm::vec3 start = cameraPos;
    glm::vec3 end = start; start += step;

    auto getdist = [&](float x1, float z1, float x2, float z2) {
        return sqrt((x1 - x2) * (x1 - x2) + (z1 - z2) * (z1 - z2));
    };

    auto getheight = [&](glm::vec3 &pd) {
        float x1 = floor(pd.x * 10) / 10.0f, z1 = floor(pd.z * 10) / 10.0f;
        float d1 = 1.0f / getdist((float)pd.x, (float)pd.z, x1, z1);
        float d2 = 1.0f / getdist((float)pd.x, (float)pd.z, x1, z1 + 0.1f);
        float d3 = 1.0f / getdist((float)pd.x, (float)pd.z, x1 + 0.1f, z1);
        float d4 = 1.0f / getdist((float)pd.x, (float)pd.z, x1 + 0.1f, z1 + 0.1f);
        float sum = d1 + d2 + d3 + d4;
        return d1 / sum * geth[{x1, z1}] + d2 / sum * geth[{x1, z1 + 0.1f}] + d3 / sum * geth[{x1 + 0.1f, z1}] + d4 / sum * geth[{x1 + 0.1f, z1 + 0.1f}];
    };
    
    float h1 = getheight(cameraPos);
    while (start.y > h1 && h1 >= 0) {
        end = start;
        start += step;
        h1 = getheight(start);
    }

    glm::vec3 intersection = {0, 0, 0};
    if (h1 >= 0)
    {
        for (int i = 0; i < 32; i++) {
            glm::vec3 mid = (start + end) * 0.5f;
            if (getheight(mid) < h1) {
                end = mid;
            } else {
                start = mid;
            }
        }
        glm::vec3 collision = (start + end) * 0.5f;
        intersection = collision; 
    }

    if (intersection.x > (h - 1) * 10 || intersection.x < 0 || intersection.z > (w - 1) * 10 || intersection.z < 0) {
        return;
    }
    generateModelAtPoint(intersection);
}

void SceneEditorApp::dealDelModel(MouseBtn button)
{
    if ((button != MouseBtn::k_btnLeft) || is_alt_down()) {
        return;
    }
    m_renderer.removeModel(m_currModel);
    m_currModel = m_renderer.getLastModel();
}

void SceneEditorApp::dealChangeSkyBox(MouseBtn button)
{
    if ((button != MouseBtn::k_btnLeft) || is_alt_down()) {
        return;
    }
    int index = SceneEditorApp::setSkyBoxIndex();
    m_renderer.setSkyBox(m_skyBoxList[index]);
}

void SceneEditorApp::dealTurnOffLight(MouseBtn button)
{
    if ((button != MouseBtn::k_btnLeft) || is_alt_down()) {
        return;
    }
    m_renderer.removeLight(m_spotLight);
}

void SceneEditorApp::dealTurnOnLight(MouseBtn button)
{
    if ((button != MouseBtn::k_btnLeft) || is_alt_down()) {
        return;
    }
    m_renderer.removeLight(m_spotLight);
    m_renderer.addLight(m_spotLight);
}

void SceneEditorApp::dealTurnDownLight(MouseBtn button)
{
    if ((button != MouseBtn::k_btnLeft) || is_alt_down()) {
        return;
    }
    m_renderer.removeLight(m_spotLight);
    m_spotLight->TurnDownColor();
    m_renderer.addLight(m_spotLight);

}


void SceneEditorApp::dealTurnUpLight(MouseBtn button)
{
    if ((button != MouseBtn::k_btnLeft) || is_alt_down()) {
        return;
    }
    m_renderer.removeLight(m_spotLight);
    m_spotLight->TurnUpColor();
    m_renderer.addLight(m_spotLight);

}

void SceneEditorApp::dealTurnOffLight2(MouseBtn button)
{
    if ((button != MouseBtn::k_btnLeft) || is_alt_down()) {
        return;
    }
    m_renderer.removeLight(m_dirLight);
}

void SceneEditorApp::dealTurnOnLight2(MouseBtn button)
{
    if ((button != MouseBtn::k_btnLeft) || is_alt_down()) {
        return;
    }
    m_renderer.removeLight(m_dirLight);
    m_renderer.addLight(m_dirLight);
}

void SceneEditorApp::dealTurnDownLight2(MouseBtn button)
{
    if ((button != MouseBtn::k_btnLeft) || is_alt_down()) {
        return;
    }
    m_renderer.removeLight(m_dirLight);
    m_dirLight->TurnDownColor();
    m_renderer.addLight(m_dirLight);

}


void SceneEditorApp::dealTurnUpLight2(MouseBtn button)
{
    if ((button != MouseBtn::k_btnLeft) || is_alt_down()) {
        return;
    }
    m_renderer.removeLight(m_dirLight);
    m_dirLight->TurnUpColor();
    m_renderer.addLight(m_dirLight);

}
void SceneEditorApp::setModelIndex(int index)
{
    m_modelBufferIndex = index;
}
int SceneEditorApp::setSkyBoxIndex()
{
    if(m_skyBoxIndex < m_skyBoxList.size()-1)
    {
        m_skyBoxIndex += 1;
    }
    else
    {
        m_skyBoxIndex = 0;
    }
    return m_skyBoxIndex;
}
void SceneEditorApp::generateModelAtPoint(const glm::vec3 &point)
{
    if (m_modelBufferIndex < m_modelList.size()) {
        m_currModel = make_shared<Model>();
        m_currModel->SetAccessor(m_modelList[m_modelBufferIndex].get());
        m_mPos = glm::translate(mat4(1.0), point);
        m_currModel->setTranslateMatrix(m_mPos);
        m_renderer.addModel(m_currModel);
    }
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
        move[2] -= 2;
    }
    if (st[key_s]) {
        move[2] += 2;
    }
    if (st[key_a]) {
        move[0] -= 2;
    }
    if (st[key_d]) {
        move[0] += 2;
    }
    // if (st[SDLK_DOWN]) {
    //     move[1] -= 0.2;
    // }
    // if (st[SDLK_UP]) {
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

void SceneEditorApp::dealLoadModel(const std::string &model)
{
    auto m = make_shared<ModelBuffer>();
    m->loadModel(model);
    m_modelList.push_back(m);
}


void SceneEditorApp::dealScaleModel(const glm::mat4 &mat)
{
    m_mScale = mat;
    if (!m_currModel) {
        return;
    }
    m_currModel->setBaseMatrix(m_mRotate * m_mScale);
}

void SceneEditorApp::dealRotateModel(const glm::mat4 &mat)
{
    m_mRotate = mat;
    if (!m_currModel) {
        return;
    }
    m_currModel->setBaseMatrix(m_mRotate * m_mScale);
}

void SceneEditorApp::dealMoveModel(const glm::mat4 &mat)
{
    m_mMove = mat;
    if (!m_currModel) {
        return;
    }
    m_currModel->setTranslateMatrix(m_mMove * m_mPos);
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