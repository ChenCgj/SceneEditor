#include <iostream>
#include "gtc/matrix_transform.hpp"
#include "debug.h"
#include "sceneEditorApp.h"
#include "SUI.h"
#include "fsm.h"
#include<io.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace sui;

GraphicArg *arg = nullptr;

constexpr int k_width = 1280;
constexpr int k_height = 720;

static void init(void *arg);
static void render(void *arg);
static void clean(void *arg);

static void generateModelImage();

static Grid_pane *generateModelOperateMenu();
// static Grid_pane *generateModelOperateMenu2(vector<string> fileVec);
static Pane *generateModelPic();
static Pane *generateSkyPic();

int main(int argc, char *argv[])
{
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
    pWnd->add_node(generateSkyPic());
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
        args->m_app->dealLoadModel("..\\resource\\models\\house\\wood_flat_house.obj");
        args->m_app->dealLoadModel("..\\resource\\models\\mushroom\\russula_low.obj");
        args->m_app->dealLoadModel("..\\resource\\models\\stone\\R6\\Rock_6.OBJ");
        args->m_app->dealLoadModel("..\\resource\\models\\tree\\uploads_files_3461467_tree.obj");
        args->m_app->dealLoadModel("..\\resource\\models\\tree\\crismas-tree.obj");
        args->m_app->dealLoadModel("..\\resource\\models\\grass\\file.obj");
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

static Pane *generateSkyPic()
{
    Pane *pane = new Pane(1100,100,20,200);
    Button *sky = new Button("change sky",1010,270,120,20);
    Button *back = new Button("return",1010,270,120,20);
    pane->add_node(sky);
    pane->add_node(back);
    pane->add_content(*sky,10,20);
    pane->add_content(*back,10,40);
    sky->add_listener(dealChangeSkyBox,Button::Button_event::be_up, arg);
    back->add_listener(dealDelModel,Button::Button_event::be_up, arg);
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
    img->load_img("..\\resource\\pic\\pic1.png");
    arg->images.push_back(img);
    img = new Image(100, 100);
    img->load_img("..\\resource\\pic\\pic2.png");
    arg->images.push_back(img);
    img = new Image(100, 100);
    img->load_img("..\\resource\\pic\\pic3.png");
    arg->images.push_back(img);
    img = new Image(100, 100);
    img->load_img("..\\resource\\pic\\pic4.png");
    arg->images.push_back(img);
    img = new Image(100, 100);
    img->load_img("..\\resource\\pic\\pic5.png");
    arg->images.push_back(img);
    img = new Image(100, 100);
    img->load_img("..\\resource\\pic\\pic6.png");
    arg->images.push_back(img);
    img = new Image(100, 100);
    img->load_img("..\\resource\\pic\\pic7.png");
    arg->images.push_back(img);
}