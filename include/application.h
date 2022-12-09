#ifndef APPLICATION_H
#define APPLICATION_H
#include <utility>
#include <unordered_set>
#include <string>
#include <vector>
#include "SDL_keycode.h"
#include "SDL_events.h"

class Application {
public:
    Application(const std::string &title, int width, int height, bool fullScreen = false);
    virtual ~Application();
    virtual int exec();
protected:
    enum MouseBtn {k_btnLeft, k_btnMiddle, k_btnRight};
    virtual bool render() = 0;
    virtual void dealButtonDown(const std::pair<int, int> &pos, MouseBtn button);
    virtual void dealButtonUp(const std::pair<int, int> &pos, MouseBtn button);
    virtual void dealMouseMove(const std::pair<int, int> &pos, const std::pair<int, int> &rpos);
    /**
    * @param pos the mouse pos
    * @param scroll the wheel direction, scroll.first is the x dir, scroll.second is the y dir
    *               -1 is left and up, 1 is right and down
    */
    virtual void dealWheel(const std::pair<int, int> &pos, const std::pair<int, int> &scroll);
    virtual void dealKeyDown(SDL_Keycode key);
    virtual void dealKeyUp(SDL_Keycode key);
    virtual void dealWinSizeChange(const std::pair<int, int> &size);
    virtual void dealOther(const SDL_Event *pe);
    std::pair<int, int> getWindowSize() const;
    /**
    * @return keys which are pressed
    */
    const std::unordered_set<SDL_Keycode> &keyBoardStatu() const;
    bool leftButtonPressed() const;
    bool rightButtonPressed() const;
    bool middleButtonPressed() const;
    bool altKeyPressed() const;
private:
    void dealEvent(SDL_Event *e);
    std::unordered_set<SDL_Keycode> m_keyStatus;
    std::vector<int> m_mouseStatus;
    SDL_Window *m_pWnd;
    SDL_GLContext m_context;
};

#endif