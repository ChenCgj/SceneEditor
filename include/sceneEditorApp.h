#ifndef SCENEEDITORAPP_H
#define SCENEEDITORAPP_H

#include <memory>
#include "application.h"
#include "camera.h"
#include "model.h"
#include "shader.h"
#include "skyBox.h"
#include "light.h"
#include "renderer.h"
#include "SUI.h"

class Mesh;

class SceneEditorApp {
    friend class sui::Opengl_graphic;
public:
    SceneEditorApp(int width, int height);
    ~SceneEditorApp();
    enum MouseBtn {k_btnLeft, k_btnMiddle, k_btnRight};
    bool render();
    bool getStatus();
    void setStatus();
    void dealButtonDown(const std::pair<int, int> &pos, MouseBtn button);
    void dealDelModel(MouseBtn button);
    void dealChangeSkyBox(MouseBtn button);
    void dealTurnOffLight(MouseBtn button);
    void dealTurnOnLight(MouseBtn button);
    void dealTurnDownLight(MouseBtn button);
    void dealTurnUpLight(MouseBtn button);
    void dealTurnOffLight2(MouseBtn button);
    void dealTurnOnLight2(MouseBtn button);
    void dealTurnDownLight2(MouseBtn button);
    void dealTurnUpLight2(MouseBtn button);
    // void dealButtonUp(const std::pair<int, int> &pos, MouseBtn button);
    void dealMouseMove(const std::pair<int, int> &pos, const std::pair<int, int> &rpos);
    void dealWheel(const std::pair<int, int> &pos, float scroll);
    void dealKeyDown(sui::Key_code key);
    // void dealKeyUp(SDL_Keycode key);
    void dealWinSizeChange(const std::pair<int, int> &size);
    // void dealOther(const SDL_Event *pe);
    void dealLoadModel(const std::string &model);
    void dealScaleModel(const glm::mat4 &mat);
    void dealRotateModel(const glm::mat4 &mat);
    void dealMoveModel(const glm::mat4 &mat);
    void setModelIndex(int index);
    int setSkyBoxIndex();
protected:
private:
    void generateModelAtPoint(const glm::vec3 &point);
    int m_width, m_height;
    Camera m_camera;
    Renderer m_renderer;
    std::shared_ptr<Mesh> m_mesh;
    std::vector<std::shared_ptr<ModelBuffer>> m_modelList;
    std::shared_ptr<PointLight> m_light;
    std::shared_ptr<SpotLight> m_spotLight;
    std::shared_ptr<Dirlight> m_dirLight;
    std::vector<std::shared_ptr<SkyBox>> m_skyBoxList;
    bool status;
    std::shared_ptr<Model> m_currModel;
    glm::mat4 m_mMove, m_mScale, m_mRotate, m_mPos;
    int m_modelBufferIndex;
    int m_skyBoxIndex;
};

#endif