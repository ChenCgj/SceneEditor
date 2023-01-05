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
    r = glm::rotate(r, (float)rx, glm::vec3(1, 0, 0));
    args->m_app->dealRotateModel(r);
}

void dealDelModel(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    SceneEditorApp::MouseBtn btn = SceneEditorApp::MouseBtn::k_btnLeft;
    args->m_app->dealDelModel(btn);
}

void dealChangeSkyBox(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    SceneEditorApp::MouseBtn btn = SceneEditorApp::MouseBtn::k_btnLeft;
    args->m_app->dealChangeSkyBox(btn);
}

void dealTurnOffLight(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    SceneEditorApp::MouseBtn btn = SceneEditorApp::MouseBtn::k_btnLeft;
    args->m_app->dealTurnOffLight(btn);
}

void dealTurnOnLight(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    SceneEditorApp::MouseBtn btn = SceneEditorApp::MouseBtn::k_btnLeft;
    args->m_app->dealTurnOnLight(btn);
}

void dealTurnDownLight(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    SceneEditorApp::MouseBtn btn = SceneEditorApp::MouseBtn::k_btnLeft;
    args->m_app->dealTurnDownLight(btn);
}

void dealTurnUpLight(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    SceneEditorApp::MouseBtn btn = SceneEditorApp::MouseBtn::k_btnLeft;
    args->m_app->dealTurnUpLight(btn);
}

void dealTurnOffLight2(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    SceneEditorApp::MouseBtn btn = SceneEditorApp::MouseBtn::k_btnLeft;
    args->m_app->dealTurnOffLight2(btn);
}

void dealTurnOnLight2(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    SceneEditorApp::MouseBtn btn = SceneEditorApp::MouseBtn::k_btnLeft;
    args->m_app->dealTurnOnLight2(btn);
}

void dealTurnDownLight2(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    SceneEditorApp::MouseBtn btn = SceneEditorApp::MouseBtn::k_btnLeft;
    args->m_app->dealTurnDownLight2(btn);
}

void dealTurnUpLight2(const Mouse_button_event &e, void *arg)
{
    GraphicArg *args = reinterpret_cast<GraphicArg *>(arg);
    SceneEditorApp::MouseBtn btn = SceneEditorApp::MouseBtn::k_btnLeft;
    args->m_app->dealTurnUpLight2(btn);
}