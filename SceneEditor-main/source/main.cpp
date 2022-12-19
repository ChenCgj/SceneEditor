#include <iostream>
#include "debug.h"
#include "sceneEditorApp.h"
#include "SUI.h"

using namespace std;
using namespace sui;

struct GraphicArg {
    SceneEditorApp *m_app;
    Opengl_graphic *graphic;
} *arg = nullptr;
constexpr int k_width = 1280;
constexpr int k_height = 720;

static void init(void *arg);
static void render(void *arg);
static void clean(void *arg);
static void dealMouseWheel(const Mouse_wheel_event &e, void *arg);
static void dealMouseMotion(const Mouse_motion_event &e, void *arg);
static void dealKeyboard(const Keyboard_event &e, void *arg);
static void dealButtonDown(const Mouse_button_event &e, void *arg);
static void dealLoadModel(const Mouse_button_event &e, void *arg);
static void dealRotateModel(const Mouse_button_event &e, void *arg);
static void dealMoveModel(const Mouse_button_event &e, void *arg);
static void dealScaleModel(const Mouse_button_event &e, void *arg);

static Horizontal_pane *generateMenu();
static Grid_pane *generateModelOperateMenu();

int main(int argc, char *argv[])
{
    set_run_mode(Run_mode::wait);
    Window *pWnd = new Window("SceneEditor", k_width, k_height, window_flag_opengl);
    Opengl_graphic *graphic = new Opengl_graphic(0, 0, k_width, k_height);
    // graphic->set_redraw_flag(true);
    arg = new GraphicArg{nullptr, graphic};
    graphic->add_opengl_funcs({init, render, clean}, {arg, arg, arg});
    graphic->add_listener(dealButtonDown, Opengl_graphic::Opengl_graphic_event::oge_button_down, arg);
    graphic->add_listener(dealMouseWheel, Opengl_graphic::Opengl_graphic_event::oge_wheel, arg);
    graphic->add_listener(dealMouseMotion, Opengl_graphic::Opengl_graphic_event::oge_move, arg);
    pWnd->add_listener(dealKeyboard, Key_event::ke_down, arg);
    pWnd->add_node(graphic);
    pWnd->add_node(generateMenu());
    pWnd->add_node(generateModelOperateMenu());
    pWnd->show();
    return 0;
}

void init(void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    if (args->m_app == nullptr) {
        args->m_app = new SceneEditorApp(args->graphic->get_width(), args->graphic->get_height());
    }
}

void render(void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    if (args->m_app) {
        args->m_app->render();
    }
}

void clean(void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    if (!args->m_app) {
        delete []args->m_app;
    }
    delete args;
}

void dealMouseWheel(const Mouse_wheel_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    args->m_app->dealWheel(get_mouse_pos(), get_wheel_scroll(e));
}

void dealMouseMotion(const Mouse_motion_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    args->m_app->dealMouseMove(get_mouse_pos(e), get_mouse_move(e));
}

void dealKeyboard(const Keyboard_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    args->m_app->dealKeyDown(get_key(e));
    args->graphic->set_redraw_flag(true);
    present_all();
}

void dealButtonDown(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    SceneEditorApp::MouseBtn btn = is_mouse_left_button_down() ? SceneEditorApp::MouseBtn::k_btnLeft :
                                   is_mouse_right_button_down() ? SceneEditorApp::MouseBtn::k_btnRight :
                                   SceneEditorApp::MouseBtn::k_btnMiddle;
    args->m_app->dealButtonDown(get_mouse_pos(), btn);
}

static Horizontal_pane *generateMenu()
{
    Horizontal_pane *pane = new Horizontal_pane(20, 20, k_width, 20);
    Button *pbLoadModel = new Button("Load Model", 0, 0, 100, 20);
    pbLoadModel->add_listener(dealLoadModel, Button::Button_event::be_up, arg);
    pane->add_node(pbLoadModel);
    pane->add_content(*pbLoadModel);
    return pane;
}

static Grid_pane *generateModelOperateMenu()
{
    Grid_pane *pane = new Grid_pane(1000, 650, 200, 50, 3, 2);
    Button *pbZoomIn = new Button("zoom in", 0, 0, 80, 30);
    Button *pbZoomOut = new Button("zoom in", 0, 0, 80, 30);
    Button *pbMoveLeft = new Button("move left", 0, 0, 80, 30);
    Button *pbMoveRight = new Button("move right", 0, 0, 80, 30);
    Button *pbRotateX = new Button("rotate X", 0, 0, 80, 30);
    Button *pbRotateY = new Button("rotate Y", 0, 0, 80, 30);

    pbZoomIn->add_listener(dealScaleModel, Button::Button_event::be_up, arg);
    pbZoomOut->add_listener(dealScaleModel, Button::Button_event::be_up, arg);
    pbMoveLeft->add_listener(dealMoveModel, Button::Button_event::be_up, arg);
    pbMoveRight->add_listener(dealMoveModel, Button::Button_event::be_up, arg);
    pbRotateX->add_listener(dealRotateModel, Button::Button_event::be_up, arg);
    pbRotateY->add_listener(dealRotateModel, Button::Button_event::be_up, arg);

    pane->add_nodes({pbZoomIn, pbZoomOut, pbMoveLeft, pbMoveRight, pbRotateX, pbRotateY});
    pane->add_content(*pbZoomIn, 0, 0);
    pane->add_content(*pbZoomOut, 0, 1);
    pane->add_content(*pbMoveLeft, 1, 0);
    pane->add_content(*pbMoveRight, 1, 1);
    pane->add_content(*pbRotateX, 2, 0);
    pane->add_content(*pbRotateY, 2, 1);
    return pane;
}

void dealLoadModel(const Mouse_button_event &e, void *arg)
{
    LOGINFO("Load model...");
}

void dealRotateModel(const Mouse_button_event &e, void *arg)
{
    LOGINFO("rotate model");
}

void dealMoveModel(const Mouse_button_event &e, void *arg)
{
    LOGINFO("move model");
}

void dealScaleModel(const Mouse_button_event &e, void *arg)
{
    LOGINFO("scale model");
}
