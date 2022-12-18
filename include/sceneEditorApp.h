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
    void dealButtonDown(const std::pair<int, int> &pos, MouseBtn button);
    // void dealButtonUp(const std::pair<int, int> &pos, MouseBtn button);
    void dealMouseMove(const std::pair<int, int> &pos, const std::pair<int, int> &rpos);
    void dealWheel(const std::pair<int, int> &pos, float scroll);
    void dealKeyDown(sui::Key_code key);
    // void dealKeyUp(SDL_Keycode key);
    void dealWinSizeChange(const std::pair<int, int> &size);
    // void dealOther(const SDL_Event *pe);
protected:
private:
    int m_width, m_height;
    Camera m_camera;
    Renderer m_renderer;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Model> m_model;
    std::shared_ptr<PointLight> m_light;
    std::shared_ptr<SpotLight> m_spotLight;
    std::shared_ptr<Dirlight> m_dirLight;
    std::shared_ptr<SkyBox> m_skyBox;
};

#endif