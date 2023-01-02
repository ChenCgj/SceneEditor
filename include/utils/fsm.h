#ifndef FSM_H
#define FSM_H

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
};
void dealMouseWheel(const Mouse_wheel_event &e, void *arg);
void dealMouseMotion(const Mouse_motion_event &e, void *arg);
void dealKeyboard(const Keyboard_event &e, void *arg);
void dealButtonDown(const Mouse_button_event &e, void *arg);
void dealScale(const Mouse_motion_event &e, void *arg);
void dealMoveX(const Mouse_motion_event &e, void *arg);
void dealMoveY(const Mouse_motion_event &e, void *arg);
void dealMoveZ(const Mouse_motion_event &e, void *arg);
void dealRotateX(const Mouse_motion_event &e, void *arg);
void dealRotateY(const Mouse_motion_event &e, void *arg);
void dealRotateZ(const Mouse_motion_event &e, void *arg);
void dealDelModel(const Mouse_button_event &e, void *arg);
void dealChangeSkyBox(const Mouse_button_event &e, void *arg);
#endif