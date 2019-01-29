#include "MxEvent.h"

namespace Mix {
    void MxEvent::onEvent(SDL_Event& _event) {
        switch(_event.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                std::string scancode(SDL_GetScancodeName(_event.key.keysym.scancode));
                std::string keycode(SDL_GetKeyName(_event.key.keysym.sym));
                unsigned int timestamp = _event.key.timestamp;
                unsigned int windowID = _event.key.windowID;
                if(_event.type == SDL_KEYDOWN) {
                    bool isRepeat = _event.key.repeat == 0 ? false : true;
                    onKeyDown(scancode, keycode, isRepeat, timestamp, windowID);
                }
                else
                    onKeyUp(scancode, keycode, timestamp, windowID);
                break;
            }
            case SDL_MOUSEMOTION:
                if(SDL_GetRelativeMouseMode() == SDL_FALSE)
                    onMouseMotion(_event.motion.x, _event.motion.y, _event.motion.state,
                                  _event.motion.timestamp, _event.motion.windowID);
                else
                    onMouseMotion(_event.motion.xrel, _event.motion.yrel, _event.motion.state,
                                  _event.motion.timestamp, _event.motion.windowID);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                int btnIndex = SDL_BUTTON(_event.button.button);
                if(_event.type == SDL_MOUSEBUTTONDOWN)
                    onMouseBtnDown(_event.button.x, _event.button.y, btnIndex, _event.button.clicks,
                                   _event.button.timestamp, _event.button.windowID);
                else
                    onMouseBtnUp(_event.button.x, _event.button.y, btnIndex, _event.button.clicks,
                                 _event.button.timestamp, _event.button.windowID);
                break;
            }
            case SDL_MOUSEWHEEL:
            {
                int y = _event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? -1 : 1;
                y *= _event.wheel.y;
                onMouseWheel(_event.wheel.x, y, _event.wheel.timestamp, _event.wheel.windowID);
                break;
            }
            case SDL_QUIT:
                onQuit(_event.quit.timestamp);
                break;
            case SDL_USEREVENT:
                onUserEvent();
                break;
            case SDL_WINDOWEVENT:
            {
                unsigned int timestamp = _event.window.timestamp;
                unsigned int windowID = _event.window.windowID;
                switch(_event.window.event) {
                    case SDL_WINDOWEVENT_CLOSE:
                        onWindowClose(timestamp, windowID);
                        break;
                    case SDL_WINDOWEVENT_EXPOSED:
                        onWindowExposed(timestamp, windowID);
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        onKeyboardFocusGained(timestamp, windowID);
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        onKeyboardFocusLost(timestamp, windowID);
                        break;
                    case SDL_WINDOWEVENT_ENTER:
                        onMouseFocusGained(timestamp, windowID);
                        break;
                    case SDL_WINDOWEVENT_LEAVE:
                        onMouseFocusLost(timestamp, windowID);
                        break;
                    default:
                        break;
                }
            }
            default:
                break;
        }
    }
}
