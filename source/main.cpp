#include <iostream>
#include "SDL.h"
#include "debug.h"
#include "sceneEditorApp.h"

using namespace std;

static bool initSDL();
static void quitSDL();

int main(int argc, char *argv[])
{
    if (!initSDL()) {
        return -1;
    }
    SceneEditorApp app(1080, 720);
    int ret = app.exec();
    quitSDL();
    return ret;
}

bool initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        ERRINFO("Init fail. SDL: %s", SDL_GetError());
        return false;
    }
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) < 0 ||
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5) < 0 ||
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8) < 0) {
        ERRINFO("Set opengl attribute fail. SDL: %s", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return false;
    }
    return true;
}

void quitSDL()
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}