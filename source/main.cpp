#include <iostream>
#include "gtc/matrix_transform.hpp"
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
    vector<Image*> images;
    Slider *scale, *movex, *movey, *movez, *rotatex, *rotatey, *rotatez;
    int index;
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
static void dealScale(const Mouse_motion_event &e, void *arg);
static void dealMoveX(const Mouse_motion_event &e, void *arg);
static void dealMoveY(const Mouse_motion_event &e, void *arg);
static void dealMoveZ(const Mouse_motion_event &e, void *arg);
static void dealRotateX(const Mouse_motion_event &e, void *arg);
static void dealRotateY(const Mouse_motion_event &e, void *arg);
static void dealRotateZ(const Mouse_motion_event &e, void *arg);

static void generateModelImage();

static Grid_pane *generateModelOperateMenu();
// static Grid_pane *generateModelOperateMenu2(vector<string> fileVec);
static Pane *generateModelPic();

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
    set_run_mode(Run_mode::poll);
    Window *pWnd = new Window("SceneEditor", k_width, k_height, window_flag_opengl);
    Opengl_graphic *graphic = new Opengl_graphic(0, 0, k_width, k_height);
    graphic->set_always_redraw(true);
    arg = new GraphicArg{nullptr, graphic, {}, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    generateModelImage();

    graphic->add_opengl_funcs({init, render, clean}, {arg, arg, arg});
    graphic->add_listener(dealButtonDown, Opengl_graphic::Opengl_graphic_event::oge_button_down, arg);
    graphic->add_listener(dealMouseWheel, Opengl_graphic::Opengl_graphic_event::oge_wheel, arg);
    graphic->add_listener(dealMouseMotion, Opengl_graphic::Opengl_graphic_event::oge_move, arg);
    pWnd->add_listener(dealKeyboard, Key_event::ke_down, arg);
    pWnd->add_node(graphic);
    pWnd->add_node(generateModelOperateMenu());
    pWnd->add_node(generateModelPic());
    // pWnd->add_node(generateModelOperateMenu2(fileVec));
    pWnd->show();
    return 0;
}

void init(void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    if (args->m_app == nullptr) {
        args->m_app = new SceneEditorApp(args->graphic->get_width(), args->graphic->get_height());
        args->m_app->dealLoadModel("..\\resource\\models\\low_poly_tree\\Lowpoly_tree_sample.obj");
        // args->m_app->dealLoadModel("..\\resource\\models\\mustang_gt\\Textures\\mustang_GT.obj");
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
    for (auto img : args->images) {
        delete img;
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
    Grid_pane *pane = new Grid_pane(500, 480, 500, 100, 3, 2);
    Label *labelz = new Label("Zoom", 18, 0, 0, 100, 20);
    Slider *sliderz = new Slider(0, 0, 100, 20, -10, 10);
    Label *labelmx = new Label("Move X", 18, 0, 0, 100, 20);
    Slider *slidermx = new Slider(0, 0, 100, 20, -10, 10);
    Label *labelmy = new Label("Move Y", 18, 0, 0, 100, 20);
    Slider *slidermy = new Slider(0, 0, 100, 20, -10, 10);
    Label *labelmz = new Label("Move Z", 18, 0, 0, 100, 20);
    Slider *slidermz = new Slider(0, 0, 100, 20, -10, 10);
    Label *labelrx = new Label("Rotate X", 18, 0, 0, 100, 20);
    Slider *sliderrx = new Slider(0, 0, 100, 20, 0, 360);
    Label *labelry = new Label("Rotate Y", 18, 0, 0, 100, 20);
    Slider *sliderry = new Slider(0, 0, 100, 20, 0, 360);
    Label *labelrz = new Label("Rotate Z", 18, 0, 0, 100, 20);
    Slider *sliderrz = new Slider(0, 0, 100, 20, 0, 360);
    pane->add_nodes({labelz, labelmx, labelmy, labelmz, labelrx, labelry, labelrz});
    pane->add_nodes({sliderz, slidermx, slidermy, slidermz, sliderrx, sliderry, sliderrz});
    pane->add_content(*labelz, 0, 0);
    pane->add_content(*sliderz, 0, 1);
    pane->add_content(*labelmx, 1, 0);
    pane->add_content(*slidermx, 1, 1);
    pane->add_content(*labelmy, 2, 0);
    pane->add_content(*slidermy, 2, 1);
    pane->add_content(*labelmz, 3, 0);
    pane->add_content(*slidermz, 3, 1);
    pane->add_content(*labelrx, 4, 0);
    pane->add_content(*sliderrx, 4, 1);
    pane->add_content(*labelry, 5, 0);
    pane->add_content(*sliderry, 5, 1);
    pane->add_content(*labelrz, 6, 0);
    pane->add_content(*sliderrz, 6, 1);
    sliderz->set_value(1);
    slidermx->set_value(0);
    slidermy->set_value(0);
    slidermz->set_value(0);
    sliderrx->set_value(0);
    sliderry->set_value(0);
    sliderrz->set_value(0);
    sliderz->add_listener(dealScale, Slider::Slider_event::se_slide, arg);
    slidermx->add_listener(dealMoveX, Slider::Slider_event::se_slide, arg);
    slidermy->add_listener(dealMoveY, Slider::Slider_event::se_slide, arg);
    slidermz->add_listener(dealMoveZ, Slider::Slider_event::se_slide, arg);
    sliderrx->add_listener(dealRotateX, Slider::Slider_event::se_slide, arg);
    sliderry->add_listener(dealRotateY, Slider::Slider_event::se_slide, arg);
    sliderrz->add_listener(dealRotateZ, Slider::Slider_event::se_slide, arg);
    arg->scale = sliderz;
    arg->movex = slidermx;
    arg->movey = slidermy;
    arg->movez = slidermz;
    arg->rotatex = sliderrx;
    arg->rotatey = sliderry;
    arg->rotatez = sliderrz;
    return pane;
}

static Pane *generateModelPic()
{
    Pane *pane = new Pane(1000, 400, 100, 130);
    Button *before = new Button("<", 1010, 570, 30, 20);
    Button *after = new Button(">", 1060, 570, 30, 20);
    pane->add_node(before);
    pane->add_node(after);
    pane->add_content(*before, 10, 110);
    pane->add_content(*after, 60, 110);

    Graphic_board *board = new Graphic_board(0, 0, 100, 100);
    board->set_draw_color({255, 125, 125, 255});
    board->clear();
    board->draw_image(*arg->images[0], 0, 0);
    pane->add_node(board);
    arg->index = 0;
    // Button *pic = new Button("a", 0, 0, 100, 100);
    // pic->set_background_color(0, 0, 0, 0);
    // pic->set_background_image("..\\resource\\pic\\pic3.jpg", Rect(0, 0, 600, 600), Rect(0, 0, 2000, 2000), Element_status::button_normal);
    // pic->set_background_fill_style(Background_fill_style::full, dynamic_cast<Geometry *>(pic), Element_status::button_normal);
    // pane->add_node(pic);
    pane->add_content(*board, 0, 0);
    before->add_listener([=](const Mouse_button_event &, void *arg){
        GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
        if (args->index > 0 && args->images.size() != 0) {
            args->index--;
            args->m_app->setModelIndex(args->index);
            board->clear_draw_operation();
            board->draw_image(*args->images[args->index], 0, 0);
            board->set_redraw_flag(true);
        }
    }, Button::Button_event::be_up, arg);
    after->add_listener([=](const Mouse_button_event &, void *arg){
        GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
        if (args->index + 1 < int(args->images.size())) {
            args->index++;
            args->m_app->setModelIndex(args->index);
            board->clear_draw_operation();
            board->draw_image(*args->images[args->index], 0, 0);
            board->set_redraw_flag(true);
        }
    }, Button::Button_event::be_up, arg);
    return pane;
}


static void generateModelImage()
{
    Image *img = new Image(100, 100);
    img->load_img("..\\resource\\pic\\pic1.jpg");
    arg->images.push_back(img);
    img = new Image(100, 100);
    img->load_img("..\\resource\\pic\\pic2.jpg");
    arg->images.push_back(img);
}

void dealScale(const Mouse_motion_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    double scale = args->scale->get_value();
    if (scale < -1) {
        scale = -1 / scale;
    } else if (scale > 1) {
        scale = scale;
    } else {
        scale = 1;
    }
    args->m_app->dealScaleModel(glm::scale(glm::mat4(1.0), glm::vec3(scale)));
}

void dealMoveX(const Mouse_motion_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    double dx = args->movex->get_value();
    double dy = args->movey->get_value();
    double dz = args->movez->get_value();
    args->m_app->dealMoveModel(glm::translate(glm::mat4(1.0), glm::vec3(dx, dy, dz)));
}

void dealMoveY(const Mouse_motion_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    double dx = args->movex->get_value();
    double dy = args->movey->get_value();
    double dz = args->movez->get_value();
    args->m_app->dealMoveModel(glm::translate(glm::mat4(1.0), glm::vec3(dx, dy, dz)));
}

void dealMoveZ(const Mouse_motion_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    double dx = args->movex->get_value();
    double dy = args->movey->get_value();
    double dz = args->movez->get_value();
    args->m_app->dealMoveModel(glm::translate(glm::mat4(1.0), glm::vec3(dx, dy, dz)));
}

void dealRotateX(const Mouse_motion_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    double rx = args->rotatex->get_value();
    double ry = args->rotatey->get_value();
    double rz = args->rotatez->get_value();
    rx = glm::radians(rx);
    ry = glm::radians(ry);
    rz = glm::radians(rz);
    glm::mat4 r(1.0);
    r = glm::rotate(r, (float)rz, glm::vec3(0, 0, 1));
    r = glm::rotate(r, (float)ry, glm::vec3(0, 1, 0));
    r = glm::rotate(r, (float)rx, glm::vec3(1, 0, 0));
    args->m_app->dealRotateModel(r);
}

void dealRotateY(const Mouse_motion_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    double rx = args->rotatex->get_value();
    double ry = args->rotatey->get_value();
    double rz = args->rotatez->get_value();
    rx = glm::radians(rx);
    ry = glm::radians(ry);
    rz = glm::radians(rz);
    glm::mat4 r(1.0);
    r = glm::rotate(r, (float)rz, glm::vec3(0, 0, 1));
    r = glm::rotate(r, (float)ry, glm::vec3(0, 1, 0));
    r = glm::rotate(r, (float)rx, glm::vec3(1, 0, 0));
    args->m_app->dealRotateModel(r);
}

void dealRotateZ(const Mouse_motion_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    double rx = args->rotatex->get_value();
    double ry = args->rotatey->get_value();
    double rz = args->rotatez->get_value();
    rx = glm::radians(rx);
    ry = glm::radians(ry);
    rz = glm::radians(rz);
    glm::mat4 r(1.0);
    r = glm::rotate(r, (float)rz, glm::vec3(0, 0, 1));
    r = glm::rotate(r, (float)ry, glm::vec3(0, 1, 0));
    r = glm::rotate(r, (float)rz, glm::vec3(1, 0, 0));
    args->m_app->dealRotateModel(r);
}
