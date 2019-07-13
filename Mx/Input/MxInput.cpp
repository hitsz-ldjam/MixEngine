#include "MxInput.h"

namespace Mix {
    const Uint8* Input::mKeyboardState = nullptr;
    uint8_t Input::mKeyboardEvent[SDL_NUM_SCANCODES]{};
    bool Input::mAnyKey = false, Input::mAnyKeyDown = false;
    uint8_t Input::mMouseButtonEvent[SDL_BUTTON_X2]{};
    Math::Vector2i Input::mMousePosDelta(0, 0), Input::mMouseScrollDelta(0, 0);

    void Input::Awake() {
        mKeyboardState = SDL_GetKeyboardState(nullptr);
    }

    void Input::Process(const SDL_Event& _event) {
        switch(_event.type) {
            case SDL_KEYDOWN:
            {
                auto code = _event.key.keysym.scancode;
                mKeyboardEvent[code] |= mPressedMask;
                mAnyKey = true;
                if(!_event.key.repeat) {
                    mKeyboardEvent[code] |= mDownMask;
                    mAnyKeyDown = true;
                }
                break;
            }
            case SDL_KEYUP:
            {
                mKeyboardEvent[_event.key.keysym.scancode] |= mUpMask;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                //if(event.button.clicks == 1)
                mMouseButtonEvent[_event.button.button - 1] |= mDownMask;
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                mMouseButtonEvent[_event.button.button - 1] |= mUpMask;
                break;
            }
            case SDL_MOUSEMOTION:
            {
                mMousePosDelta += Math::Vector2(_event.motion.xrel, _event.motion.yrel);
                break;
            }
            case SDL_MOUSEWHEEL:
            {
                int deltaY = _event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1 : -1;
                deltaY *= _event.wheel.y;
                mMouseScrollDelta += Math::Vector2i(_event.wheel.x, deltaY);
                break;
            }
            default:
                break;
        }
    }

    void Input::Reset() {
        std::memset(mKeyboardEvent, 0, SDL_NUM_SCANCODES * sizeof(*mKeyboardEvent));
        mAnyKey = mAnyKeyDown = false;
        std::memset(mMouseButtonEvent, 0, SDL_BUTTON_X2 * sizeof(*mMouseButtonEvent));
        mMousePosDelta = mMouseScrollDelta = {0, 0};
    }
}
