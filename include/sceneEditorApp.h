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

class Mesh;

class SceneEditorApp : public Application {
public:
    SceneEditorApp(int width, int height);
    ~SceneEditorApp();
protected:
    bool render() override;
    void dealButtonDown(const std::pair<int, int> &pos, MouseBtn button) override;
    // void dealButtonUp(const std::pair<int, int> &pos, MouseBtn button) override;
    void dealMouseMove(const std::pair<int, int> &pos, const std::pair<int, int> &rpos) override;
    void dealWheel(const std::pair<int, int> &pos, const std::pair<int, int> &scroll) override;
    void dealKeyDown(SDL_Keycode key) override;
    // void dealKeyUp(SDL_Keycode key) override;
    void dealWinSizeChange(const std::pair<int, int> &size) override;
    // void dealOther(const SDL_Event *pe) override;
private:
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