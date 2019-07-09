#include "MxInput.h"

namespace Mix {
	const Uint8* Input::keyboardState = nullptr;
	Uint8 Input::keyEvent[SDL_NUM_SCANCODES] = { 0 };
	bool Input::anyKey = false;
	bool Input::anyKeyDown = false;
	Math::Vector2i Input::mouseScrollDelta = { 0, 0 };
	Math::Vector2i Input::mousePosDelta = { 0,0 };
	Uint8 Input::mouseButtonEvent[SDL_BUTTON_X2] = { 0 };

	void Input::Init() {
		keyboardState = SDL_GetKeyboardState(nullptr);
	}

	bool Input::Process(const SDL_Event& _event) {
		switch (_event.type) {
		case SDL_KEYDOWN:
		{
			SDL_Scancode code = _event.key.keysym.scancode;
			anyKey = true;
			keyEvent[code] |= PRESSED_MASK;
			if (!_event.key.repeat) {
				anyKeyDown = true;
				keyEvent[code] |= FIRST_PRESSED_MASK;
			}
			break;
		}
		case SDL_KEYUP:
		{
			keyEvent[_event.key.keysym.scancode] |= RELEASED_MASK;
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			// if(event.button.clicks == 1)
			mouseButtonEvent[_event.button.button - 1] |= FIRST_PRESSED_MASK;
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			mouseButtonEvent[_event.button.button - 1] |= RELEASED_MASK;
			break;
		}
		case SDL_MOUSEMOTION:
		{
			mousePosDelta = { _event.motion.xrel,_event.motion.yrel };
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			int deltaY = _event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1 : -1;
			deltaY *= _event.wheel.y;
			mouseScrollDelta += Math::Vector2i(_event.wheel.x, deltaY);
			break;
		}
		default:
			break;
		}
		return false;
	}

	void Input::Reset() {
		std::memset(keyEvent, 0, SDL_NUM_SCANCODES * sizeof(*keyEvent));
		anyKey = false;
		anyKeyDown = false;
		mouseScrollDelta = { 0, 0 };
		mousePosDelta = { 0,0 };
		std::memset(mouseButtonEvent, 0, SDL_BUTTON_X2 * sizeof(*mouseButtonEvent));
	}
}
