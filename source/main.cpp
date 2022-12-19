#include <iostream>
#include "debug.h"
#include "sceneEditorApp.h"
#include "SUI.h"
#include<io.h>
#include <fstream>
#include <string>
#include <vector>

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
static void dealRotateModelRX(const Mouse_button_event &e, void *arg);
static void dealMoveModelLeft(const Mouse_button_event &e, void *arg);
static void dealScaleModelIn(const Mouse_button_event &e, void *arg);
static void dealRotateModelRY(const Mouse_button_event &e, void *arg);
static void dealMoveModelRight(const Mouse_button_event &e, void *arg);
static void dealScaleModelOut(const Mouse_button_event &e, void *arg);

static Horizontal_pane *generateMenu();
static Grid_pane *generateModelOperateMenu();
static Grid_pane *generateModelOperateMenu2(vector<string> fileVec);


int FilesRead(string root, vector<string> &fileVec)
{
    int Nums = 0;
    long long handle = 0;
    struct _finddata_t fileinfo;
    string temp_str;
    if ((handle = _findfirst(temp_str.assign(root).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            if ((fileinfo.attrib&_A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    FilesRead(temp_str.assign(root).append(fileinfo.name).c_str(), fileVec);
            }
            else
            {
                try
                {
                    if (fileinfo.size == 0)
                        throw - 1;
                    else
                        fileVec.push_back(temp_str.assign("").append("\\").append(fileinfo.name));
                }
                catch (int e)
                {
                    if (e == -1)
                        cout << "file is empty!" << endl;
                }
            }
        } while (_findnext(handle, &fileinfo) == 0);
        _findclose(handle);
    }
 
    Nums = (int)fileVec.size();
    cout << "Nums: " << Nums << endl;
    if (Nums > 0)
        return Nums;
    else
        return 0;
}

int main(int argc, char *argv[])
{
    string inPath = "..\\resource\\models\\";  
    vector<string> fileVec;
    string suffix = ".obj";
    FilesRead(inPath, fileVec);
    for (int i = 0; i < fileVec.size(); i++)
    {
        string str = fileVec[i].c_str();
        if(str.compare(str.length()-suffix.length(),suffix.length(),suffix)==0)
            printf("name:%s\n", fileVec[i].c_str());
    }
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
    // pWnd->add_node(generateModelOperateMenu2(fileVec));
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

// static Grid_pane *generateModelOperateMenu2(vector<string> fileVec)
// {
//     Grid_pane *pane = new Grid_pane(20, 40, 200, 50, 5, 2);
//     Button *models[10];
//     int cnt = 0;
//     string suffix = ".obj";
//     for (int i = 0; i < 10; i++)
//     {
//         string str = fileVec[i].c_str();
//         if(str.compare(str.length()-suffix.length(),suffix.length(),suffix)==0)
//         {
//             models[cnt] = new Button(str, 0, 0, 80, 30);
//             models[cnt]->add_listener(dealScaleModelIn, Button::Button_event::be_up, arg);
//             // models.push_back(tmp);
//             // pane->add_nodes({tmp});
//             // pane->add_content(*tmp,cnt/2,cnt%2);
//             cnt++;
//         }
//         pane->add_nodes({models[0],models[1],models[2],models[3],models[4],models[5],models[6],models[7],models[8],models[9]});
//         for(int i = 0;i < 10; i++)
//         {
//             pane->add_content(*models[i],i/2,i%2);
//         }
//     }
//     return pane;
// }

static Grid_pane *generateModelOperateMenu()
{
    Grid_pane *pane = new Grid_pane(1000, 650, 200, 50, 3, 2);
    Button *pbZoomIn = new Button("zoom in", 0, 0, 80, 30);
    Button *pbZoomOut = new Button("zoom out", 0, 0, 80, 30);
    Button *pbMoveLeft = new Button("move left", 0, 0, 80, 30);
    Button *pbMoveRight = new Button("move right", 0, 0, 80, 30);
    Button *pbRotateX = new Button("rotate X", 0, 0, 80, 30);
    Button *pbRotateY = new Button("rotate Y", 0, 0, 80, 30);

    pbZoomIn->add_listener(dealScaleModelIn, Button::Button_event::be_up, arg);
    pbZoomOut->add_listener(dealScaleModelOut, Button::Button_event::be_up, arg);
    pbMoveLeft->add_listener(dealMoveModelLeft, Button::Button_event::be_up, arg);
    pbMoveRight->add_listener(dealMoveModelRight, Button::Button_event::be_up, arg);
    pbRotateX->add_listener(dealRotateModelRX, Button::Button_event::be_up, arg);
    pbRotateY->add_listener(dealRotateModelRY, Button::Button_event::be_up, arg);

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
    // todo:when load,add a button under the button"load model and you can click it to change the model"
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    args->graphic->add_listener(dealButtonDown, Opengl_graphic::Opengl_graphic_event::oge_button_down, arg);
    LOGINFO("Load model...");
    args->m_app->setStatus();
    cout << get_mouse_pos().first << endl;
    // args->m_app->dealLoadModel(trans);
}

void dealRotateModelRX(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    glm::vec3 axis = glm::vec3(1.0f,0.0f,0.0f);
    LOGINFO("rotate model");
    if(args->m_app->getStatus())
    {
         args->m_app->dealRotateModel(axis);
    }
    else
    {
        args->m_app->dealRotateModel(axis);
    }
}

void dealMoveModelLeft(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    glm::vec3 trans = glm::vec3(-1.0f,0.0f,0.0f);
    args->m_app->dealMoveModel(trans);
    LOGINFO("move model");
}

void dealScaleModelIn(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    glm::vec3 trans = glm::vec3(0.8f,0.8f,0.8f);
    args->m_app->dealScaleModel(trans);
    LOGINFO("scale model");
}

void dealRotateModelRY(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    glm::vec3 axis = glm::vec3(0.0f,1.0f,0.0f);
    args->m_app->dealRotateModel(axis);
    LOGINFO("rotate model");
}

void dealMoveModelRight(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    glm::vec3 trans = glm::vec3(1.0f,0.0f,0.0f);
    args->m_app->dealMoveModel(trans);
    LOGINFO("move model");
}

void dealScaleModelOut(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    glm::vec3 trans = glm::vec3(1.25f,1.25f,1.25f);
    args->m_app->dealScaleModel(trans);
    LOGINFO("scale model");
}
