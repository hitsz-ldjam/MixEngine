#include "MxPlantform.h"
#include "../Log/MxLog.h"
#include <SDL2/SDL.h>

namespace Mix {
	uint16_t SDLKeyModeMap(SDL_Keymod _mod) {
		switch (_mod) {
		case KMOD_LCTRL:	return PFMode_LCtrl;
		case KMOD_RCTRL:	return PFMode_RCtrl;
		case KMOD_LSHIFT:	return PFMode_LShift;
		case KMOD_RSHIFT:	return PFMode_RShift;
		case KMOD_LALT:		return PFMode_LAlt;
		case KMOD_RALT:		return PFMode_RAlt;
		case KMOD_CTRL:		return PFMode_Ctrl;
		case KMOD_SHIFT:	return PFMode_Shift;
		case KMOD_ALT:		return PfMode_Alt;
		default:return 0x00;
		}
	}

	ButtonCode SDLKeyboardMap(SDL_Scancode _scancode) {
		switch (_scancode) {
		case SDL_SCANCODE_A:			return ButtonCode::A;
		case SDL_SCANCODE_B:			return ButtonCode::B;
		case SDL_SCANCODE_C:			return ButtonCode::C;
		case SDL_SCANCODE_D:			return ButtonCode::D;
		case SDL_SCANCODE_E:			return ButtonCode::E;
		case SDL_SCANCODE_F:			return ButtonCode::F;
		case SDL_SCANCODE_G:			return ButtonCode::G;
		case SDL_SCANCODE_H:			return ButtonCode::H;
		case SDL_SCANCODE_I:			return ButtonCode::I;
		case SDL_SCANCODE_J:			return ButtonCode::J;
		case SDL_SCANCODE_K:			return ButtonCode::K;
		case SDL_SCANCODE_L:			return ButtonCode::L;
		case SDL_SCANCODE_M:			return ButtonCode::M;
		case SDL_SCANCODE_N:			return ButtonCode::N;
		case SDL_SCANCODE_O:			return ButtonCode::O;
		case SDL_SCANCODE_P:			return ButtonCode::P;
		case SDL_SCANCODE_Q:			return ButtonCode::Q;
		case SDL_SCANCODE_R:			return ButtonCode::R;
		case SDL_SCANCODE_S:			return ButtonCode::S;
		case SDL_SCANCODE_T:			return ButtonCode::T;
		case SDL_SCANCODE_U:			return ButtonCode::U;
		case SDL_SCANCODE_V:			return ButtonCode::V;
		case SDL_SCANCODE_W:			return ButtonCode::W;
		case SDL_SCANCODE_X:			return ButtonCode::X;
		case SDL_SCANCODE_Y:			return ButtonCode::Y;
		case SDL_SCANCODE_Z:			return ButtonCode::Z;

		case SDL_SCANCODE_LEFT:			return ButtonCode::Left;
		case SDL_SCANCODE_UP:			return ButtonCode::Up;
		case SDL_SCANCODE_RIGHT:		return ButtonCode::Right;
		case SDL_SCANCODE_DOWN:			return ButtonCode::Down;

		case SDL_SCANCODE_1:			return ButtonCode::Num_1;
		case SDL_SCANCODE_2:			return ButtonCode::Num_2;
		case SDL_SCANCODE_3:			return ButtonCode::Num_3;
		case SDL_SCANCODE_4:			return ButtonCode::Num_4;
		case SDL_SCANCODE_5:			return ButtonCode::Num_5;
		case SDL_SCANCODE_6:			return ButtonCode::Num_6;
		case SDL_SCANCODE_7:			return ButtonCode::Num_7;
		case SDL_SCANCODE_8:			return ButtonCode::Num_8;
		case SDL_SCANCODE_9:			return ButtonCode::Num_9;
		case SDL_SCANCODE_0:			return ButtonCode::Num_0;

		case SDL_SCANCODE_KP_1:			return ButtonCode::Np_1;
		case SDL_SCANCODE_KP_2:			return ButtonCode::Np_2;
		case SDL_SCANCODE_KP_3:			return ButtonCode::Np_3;
		case SDL_SCANCODE_KP_4:			return ButtonCode::Np_4;
		case SDL_SCANCODE_KP_5:			return ButtonCode::Np_5;
		case SDL_SCANCODE_KP_6:			return ButtonCode::Np_6;
		case SDL_SCANCODE_KP_7:			return ButtonCode::Np_7;
		case SDL_SCANCODE_KP_8:			return ButtonCode::Np_8;
		case SDL_SCANCODE_KP_9:			return ButtonCode::Np_9;
		case SDL_SCANCODE_KP_0:			return ButtonCode::Np_0;
		case SDL_SCANCODE_KP_PLUS:		return ButtonCode::Np_Add;
		case SDL_SCANCODE_KP_MINUS:		return ButtonCode::Np_Minus;
		case SDL_SCANCODE_KP_MULTIPLY:	return ButtonCode::Np_Multiply;
		case SDL_SCANCODE_KP_DIVIDE:	return ButtonCode::Np_Divide;
		case SDL_SCANCODE_KP_ENTER:		return ButtonCode::Np_Enter;
		case SDL_SCANCODE_KP_PERIOD:	return ButtonCode::Np_Decimal;

		case SDL_SCANCODE_BACKSPACE:	return ButtonCode::Backspace;
		case SDL_SCANCODE_TAB:			return ButtonCode::Tab;
		case SDL_SCANCODE_RETURN:		return ButtonCode::Enter;
		case SDL_SCANCODE_PAUSE:		return ButtonCode::Pause;
		case SDL_SCANCODE_NUMLOCKCLEAR: return ButtonCode::Numlock;
		case SDL_SCANCODE_SCROLLLOCK:	return ButtonCode::ScrollLock;
		case SDL_SCANCODE_CAPSLOCK:		return ButtonCode::Capslock;
		case SDL_SCANCODE_ESCAPE:		return ButtonCode::Escape;
		case SDL_SCANCODE_SPACE:		return ButtonCode::Space;

		case SDL_SCANCODE_INSERT:		return ButtonCode::Insert;
		case SDL_SCANCODE_DELETE:		return ButtonCode::Delete;
		case SDL_SCANCODE_HOME:			return ButtonCode::Home;
		case SDL_SCANCODE_END:			return ButtonCode::End;
		case SDL_SCANCODE_PAGEUP:		return ButtonCode::PageUp;
		case SDL_SCANCODE_PAGEDOWN:		return ButtonCode::PageDown;

		case SDL_SCANCODE_F1:			return ButtonCode::F1;
		case SDL_SCANCODE_F2:			return ButtonCode::F2;
		case SDL_SCANCODE_F3:			return ButtonCode::F3;
		case SDL_SCANCODE_F4:			return ButtonCode::F4;
		case SDL_SCANCODE_F5:			return ButtonCode::F5;
		case SDL_SCANCODE_F6:			return ButtonCode::F6;
		case SDL_SCANCODE_F7:			return ButtonCode::F7;
		case SDL_SCANCODE_F8:			return ButtonCode::F8;
		case SDL_SCANCODE_F9:			return ButtonCode::F9;
		case SDL_SCANCODE_F10:			return ButtonCode::F10;
		case SDL_SCANCODE_F11:			return ButtonCode::F11;
		case SDL_SCANCODE_F12:			return ButtonCode::F12;

		case SDL_SCANCODE_LCTRL:		return ButtonCode::LCtrl;
		case SDL_SCANCODE_RCTRL:		return ButtonCode::RCtrl;
		case SDL_SCANCODE_LSHIFT:		return ButtonCode::LShift;
		case SDL_SCANCODE_RSHIFT:		return ButtonCode::RShift;
		case SDL_SCANCODE_LALT:			return ButtonCode::LAlt;
		case SDL_SCANCODE_RALT:			return ButtonCode::RAlt;
		case SDL_SCANCODE_LGUI:			return ButtonCode::LGUI;
		case SDL_SCANCODE_RGUI:			return ButtonCode::RGUI;

		case SDL_SCANCODE_SEMICOLON:	return ButtonCode::Semicolon;
		case SDL_SCANCODE_EQUALS:		return ButtonCode::Equals;
		case SDL_SCANCODE_COMMA:		return ButtonCode::Comma;
		case SDL_SCANCODE_MINUS:		return ButtonCode::Minus;
		case SDL_SCANCODE_PERIOD:		return ButtonCode::Period;
		case SDL_SCANCODE_SLASH:		return ButtonCode::Slash;
		case SDL_SCANCODE_GRAVE:		return ButtonCode::Backquote;
		case SDL_SCANCODE_LEFTBRACKET:	return ButtonCode::LeftBracket;
		case SDL_SCANCODE_RIGHTBRACKET: return ButtonCode::RightBracket;
		case SDL_SCANCODE_BACKSLASH:	return ButtonCode::Backslash;
		case SDL_SCANCODE_APOSTROPHE:	return ButtonCode::Quote;

		default: return ButtonCode::Unknown;
		}
	}

	ButtonCode SDLMouseButtonMap(uint32_t _button) {
		switch (_button) {
		case SDL_BUTTON_LEFT:	return ButtonCode::Mouse_Left;
		case SDL_BUTTON_RIGHT:	return ButtonCode::Mouse_Right;
		case SDL_BUTTON_MIDDLE:	return ButtonCode::Mouse_Middle;
		case SDL_BUTTON_X1:		return ButtonCode::Mouse_X1;
		case SDL_BUTTON_X2:		return ButtonCode::Mouse_X2;

		default:return ButtonCode::Unknown;
		}
	}

	AxisCode SDLControllerAxisMap(SDL_GameControllerAxis _axis) {
		switch (_axis) {
		case SDL_CONTROLLER_AXIS_LEFTX:			return AxisCode::Gamepad_LeftX;
		case SDL_CONTROLLER_AXIS_LEFTY:			return AxisCode::Gamepad_LeftY;
		case SDL_CONTROLLER_AXIS_RIGHTX:		return AxisCode::Gamepad_RightX;
		case SDL_CONTROLLER_AXIS_RIGHTY:		return AxisCode::Gamepad_RightY;
		case SDL_CONTROLLER_AXIS_TRIGGERLEFT:	return AxisCode::Gamepad_LeftTrigger;
		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:	return AxisCode::Gamepad_RightTrigger;
		default:return AxisCode::Unknown;
		}
	}

	ButtonCode SDLControllerButtonMap(SDL_GameControllerButton _button) {
		switch (_button) {
		case SDL_CONTROLLER_BUTTON_A:				return ButtonCode::Gamepad_A;
		case SDL_CONTROLLER_BUTTON_B:				return ButtonCode::Gamepad_B;
		case SDL_CONTROLLER_BUTTON_X:				return ButtonCode::Gamepad_X;
		case SDL_CONTROLLER_BUTTON_Y:				return ButtonCode::Gamepad_Y;
		case SDL_CONTROLLER_BUTTON_BACK:			return ButtonCode::Gamepad_SpecialLeft;
		case SDL_CONTROLLER_BUTTON_START:			return ButtonCode::Gamepad_SpecialRight;
		case SDL_CONTROLLER_BUTTON_LEFTSTICK:		return ButtonCode::Gamepad_LS;
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK:		return ButtonCode::Gamepad_RS;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:	return ButtonCode::Gamepad_LB;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:	return ButtonCode::Gamepad_RB;
		case SDL_CONTROLLER_BUTTON_DPAD_UP:			return ButtonCode::Gamepad_Dpad_Up;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:		return ButtonCode::Gamepad_Dpad_Down;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:		return ButtonCode::Gamepad_Dpad_Left;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:		return ButtonCode::Gamepad_Dpad_Right;
		default: return ButtonCode::Unknown;
		}
	}

	uint8_t SDLMouseStateMap(uint8_t _state) {
		uint8_t result = 0;
		if (_state & SDL_BUTTON_LMASK)
			result |= PFMouseButton_Left;
		if (_state & SDL_BUTTON_RMASK)
			result |= PFMouseButton_Right;
		if (_state & SDL_BUTTON_MMASK)
			result |= PFMouseButton_Middle;
		if (_state & SDL_BUTTON_X1MASK)
			result |= PFMouseButton_X1;
		if (_state & SDL_BUTTON_X2MASK)
			result |= PFMouseButton_X2;
		return result;
	}

	bool Plantform::sInitialized = false;
	bool Plantform::sCursorVisible = false;
	bool Plantform::sRelativeMouseMode = false;
	std::string Plantform::sPlantformName = "Unknown";

	Event<void(const PFMouseMoveEventData&)>		Plantform::MouseMoveEvent;
	Event<void(const PFMouseButtonEventData&)>		Plantform::MouseButtonEvent;
	Event<void(const PFMouseWheelEventData&)>		Plantform::MouseWheelEvent;
	Event<void(const PFKeyboardEventData&)>			Plantform::KeyboardEvent;
	Event<void(const PFGamepadAxisEventData&)>		Plantform::GamepadAxisEvent;
	Event<void(const PFGamepadButtonEventData&)>	Plantform::GamepadButtonEvent;
	Event<void(const PFGamepadDeviceEventData&)>	Plantform::GamepadDeviceEvent;
	Event<void()>									Plantform::RequireQuitEvent;

	bool Plantform::Initialize() {
		if (!sInitialized) {
			if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER)) {
				Log::Error("Failed to initialize SDL2");
				sInitialized = false;
			}
			sPlantformName = SDL_GetPlatform();

			sInitialized = true;
		}
		return sInitialized;
	}

	void Plantform::ShutDown() {
		SDL_Quit();
	}

	const std::string& Plantform::GetPlantformName() {
		return sPlantformName;
	}

	void Plantform::SetCursorPositionGlobal(const Math::Vector2i& _screenPos) {
		SDL_WarpMouseGlobal(_screenPos.x, _screenPos.y);
	}

	void Plantform::HideCursor() {
		sCursorVisible = false;
		SDL_ShowCursor(false);
	}

	void Plantform::ShowCursor() {
		sCursorVisible = true;
		SDL_ShowCursor(true);
	}

	bool Plantform::IsCursorHidden() {
		return !sCursorVisible;
	}

	bool Plantform::CopyToClipboard(const std::string& _string) {
		return SDL_SetClipboardText(_string.c_str()) == 0;
	}

	bool Plantform::HasClipText() {
		return SDL_HasClipboardText();
	}

	void Plantform::SetRelativeMouseMode(bool _mode) {
		sRelativeMouseMode = _mode;
		SDL_SetRelativeMouseMode(_mode ? SDL_TRUE : SDL_FALSE);
	}

	bool Plantform::GetRelativeMouseMode() {
		return sRelativeMouseMode;
	}

	const std::string Plantform::CopyFromClipboard() {
		auto ptr = SDL_GetClipboardText();
		if (ptr) {
			std::string result(ptr);
			SDL_free(ptr);
			return result;
		}
		return std::string();
	}

	void Plantform::Update() {
		PollEvent();
	}

	void Plantform::PollEvent() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				// Window events
			case SDL_WINDOWEVENT:
				break;

				// Keyboard events
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				PFKeyboardEventData data;
				data.state = e.type == SDL_KEYDOWN ? PFButtonState_Pressed : PFButtonState_Released;
				data.key = SDLKeyboardMap(e.key.keysym.scancode);
				data.mode = SDLKeyModeMap(static_cast<SDL_Keymod>(e.key.keysym.mod));
				if (!KeyboardEvent.empty())
					KeyboardEvent.trigger(data);
			}
			break;

				// Mouse events
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				PFMouseButtonEventData data;
				data.state = e.type == SDL_MOUSEBUTTONDOWN ? PFButtonState_Pressed : PFButtonState_Released;
				data.clicks = e.button.clicks;
				data.button = SDLMouseButtonMap(e.button.button);
				data.pos = { e.button.x, e.button.y };
				if (!MouseButtonEvent.empty())
					MouseButtonEvent.trigger(data);
			}
			break;

			case SDL_MOUSEMOTION:
			{
				PFMouseMoveEventData data;
				data.state = SDLMouseStateMap(e.motion.state);
				data.pos = { e.motion.x, e.motion.y };
				data.relativePos = { e.motion.xrel, e.motion.yrel };
				if (!MouseMoveEvent.empty())
					MouseMoveEvent.trigger(data);
			}
			break;

			case SDL_MOUSEWHEEL:
			{
				PFMouseWheelEventData data;
				data.xy = { e.wheel.x, e.wheel.y };
				if (!MouseWheelEvent.empty())
					MouseWheelEvent.trigger(data);
			}
			break;

			// Game controller events
			case SDL_CONTROLLERAXISMOTION:
			{
				PFGamepadAxisEventData data;
				data.id = e.cbutton.which;
				data.axis = SDLControllerAxisMap(static_cast<SDL_GameControllerAxis>(e.caxis.axis));
				data.value = data.axis == AxisCode::Gamepad_LeftY || data.axis == AxisCode::Gamepad_RightY ?
					-Math::Normalize(e.caxis.value) : Math::Normalize(e.caxis.value);
				if (!GamepadAxisEvent.empty())
					GamepadAxisEvent.trigger(data);
			}
			break;

			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
			{
				PFGamepadButtonEventData data;
				data.id = e.cbutton.which;
				data.state = e.type == SDL_CONTROLLERBUTTONDOWN ? PFButtonState_Pressed : PFButtonState_Released;
				data.button = SDLControllerButtonMap(static_cast<SDL_GameControllerButton>(e.cbutton.button));
				if (!GamepadButtonEvent.empty())
					GamepadButtonEvent.trigger(data);
			}
			break;

			case SDL_JOYDEVICEADDED:
			case SDL_JOYDEVICEREMOVED:
			{
				PFGamepadDeviceEventData data;
				data.deviceState = e.type == SDL_JOYDEVICEADDED ? PFDeviceState_Added : PFDeviceState_Removed;
				data.which = e.cdevice.which;
				if (!GamepadDeviceEvent.empty())
					GamepadDeviceEvent.trigger(data);
			}
			break;

			case SDL_QUIT:
				if (!RequireQuitEvent.empty())
					RequireQuitEvent.trigger();
				break;
			default:
				break;
			}
		}

	}
}
