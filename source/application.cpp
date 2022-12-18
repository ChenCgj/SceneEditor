/*
#include "SDL_events.h"
#include "application.h"
#include "debug.h"
#include "glad/glad.h"

using std::pair;
using std::make_pair;
using std::string;
using std::unordered_set;

Application::Application(const string &title, int width, int height, bool fullScreen) : m_mouseStatus(3, 0), m_pWnd(nullptr), m_context(nullptr)
{
    uint32_t flag = 0;
    if (fullScreen) {
        flag |= SDL_WINDOW_FULLSCREEN;
    }
    if ((m_pWnd = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              width, height, flag | SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE)) == nullptr) {
        ERRINFO("Create window fail. SDL: %s", SDL_GetError());
        return;
    }
    if ((m_context = SDL_GL_CreateContext(m_pWnd)) == nullptr) {
        ERRINFO("Create opengl context fail. SDL: %s", SDL_GetError());
        SDL_DestroyWindow(m_pWnd);
        m_pWnd = nullptr;
        return;
    }
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        ERRINFO("load opengl function fail.");
        SDL_GL_DeleteContext(m_context);
        SDL_DestroyWindow(m_pWnd);
        m_context = nullptr;
        m_pWnd = nullptr;
        return;
    }
    SDL_ShowWindow(m_pWnd);
}

int Application::exec()
{
    if (!m_pWnd || !m_context) {
        return -1;
    }
    SDL_Event event;
    bool run = true;
    while (run) {
        if (!render()) {
            return -1;
        }
        SDL_GL_SwapWindow(m_pWnd);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
            }
            dealEvent(&event);
        }
    }
    return 0;
}

void Application::dealEvent(SDL_Event *pe)
{
    switch (pe->type) {
    case SDL_MOUSEBUTTONDOWN: {
        MouseBtn btn = pe->button.button & SDL_BUTTON_LMASK ? k_btnLeft :
                        pe->button.button & SDL_BUTTON_MMASK ? k_btnRight :
                        k_btnMiddle;
        m_mouseStatus[btn] = 1;
        dealButtonDown(make_pair(pe->button.x, pe->button.y), btn);
        break;
    }
    case SDL_MOUSEBUTTONUP: {
        MouseBtn btn = pe->button.button & SDL_BUTTON_LMASK ? k_btnLeft :
                        pe->button.button & SDL_BUTTON_MMASK ? k_btnRight :
                        k_btnMiddle;
        m_mouseStatus[btn] = 0;
        dealButtonUp(make_pair(pe->button.x, pe->button.y), btn);
        break;
    }
    case SDL_MOUSEWHEEL: {
        pair<int, int> pos;
        SDL_GetMouseState(&pos.first, &pos.second);
        dealWheel(pos, make_pair(pe->wheel.x, pe->wheel.y));
        break;
    }
    case SDL_MOUSEMOTION:
        dealMouseMove(make_pair(pe->motion.x, pe->motion.y), make_pair(pe->motion.xrel, pe->motion.yrel));
        break;
    case SDL_KEYDOWN: {
        m_keyStatus.insert(pe->key.keysym.sym);
        dealKeyDown(pe->key.keysym.sym);
        break;
    }
    case SDL_KEYUP: {
        m_keyStatus.erase(pe->key.keysym.sym);
        dealKeyUp(pe->key.keysym.sym);
        break;
    }
    case SDL_WINDOWEVENT:
        if (pe->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            dealWinSizeChange(make_pair(pe->window.data1, pe->window.data2));
        }
        break;
    default:
        dealOther(pe);
        break;
    }
}

void Application::dealButtonDown(const std::pair<int, int> &pos, MouseBtn button)
{
    // DBGINFO(__func__);
}

void Application::dealButtonUp(const std::pair<int, int> &pos, MouseBtn button)
{
    // DBGINFO(__func__);
}

void Application::dealMouseMove(const std::pair<int, int> &pos, const std::pair<int, int> &rpos)
{
    // DBGINFO(__func__);
}

void Application::dealWheel(const std::pair<int, int> &pos, const std::pair<int, int> &scroll)
{
    // DBGINFO(__func__);
}

void Application::dealKeyDown(SDL_Keycode key)
{
    // DBGINFO(__func__);
}

void Application::dealKeyUp(SDL_Keycode key)
{
    // DBGINFO(__func__);
}

void Application::dealWinSizeChange(const std::pair<int, int> &size)
{
    // DBGINFO(__func__);
}

void Application::dealOther(const SDL_Event *pe)
{
    // DBGINFO(__func__);
}

pair<int, int> Application::getWindowSize() const
{
    int w, h;
    SDL_GetWindowSize(m_pWnd, &w, &h);
    return make_pair(w, h);
}

const unordered_set<SDL_Keycode> &Application::keyBoardStatu() const
{
    return m_keyStatus;
}

bool Application::leftButtonPressed() const
{
    return m_mouseStatus[k_btnLeft];
}

bool Application::rightButtonPressed() const
{
    return m_mouseStatus[k_btnRight];
}

bool Application::middleButtonPressed() const
{
    return m_mouseStatus[k_btnMiddle];
}

bool Application::altKeyPressed() const
{
    return SDL_GetModState() & KMOD_ALT;
}

Application::~Application()
{
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_pWnd);
    m_context = nullptr;
    m_pWnd = nullptr;
}
*/