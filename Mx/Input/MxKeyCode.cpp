#include "MxKeyCode.h"

namespace Mix {

	std::string ToString(ButtonCode _button) {
		switch (_button) {
		case ButtonCode::A: return "A";
		case ButtonCode::B: return "B";
		case ButtonCode::C: return "C";
		case ButtonCode::D: return "D";
		case ButtonCode::E: return "E";
		case ButtonCode::F: return "F";
		case ButtonCode::G: return "G";
		case ButtonCode::H: return "H";
		case ButtonCode::I: return "I";
		case ButtonCode::J: return "J";
		case ButtonCode::K: return "K";
		case ButtonCode::L: return "L";
		case ButtonCode::M: return "M";
		case ButtonCode::N: return "N";
		case ButtonCode::O: return "O";
		case ButtonCode::P: return "P";
		case ButtonCode::Q: return "Q";
		case ButtonCode::R: return "R";
		case ButtonCode::S: return "S";
		case ButtonCode::T: return "T";
		case ButtonCode::U: return "U";
		case ButtonCode::V: return "V";
		case ButtonCode::W: return "W";
		case ButtonCode::X: return "X";
		case ButtonCode::Y: return "Y";
		case ButtonCode::Z: return "Z";
		case ButtonCode::Left:	return "Left";
		case ButtonCode::Up:	return "Up";
		case ButtonCode::Right: return "Right";
		case ButtonCode::Down:	return "Down";
		case ButtonCode::Num_1: return "Num_1";
		case ButtonCode::Num_2: return "Num_2";
		case ButtonCode::Num_3: return "Num_3";
		case ButtonCode::Num_4: return "Num_4";
		case ButtonCode::Num_5: return "Num_5";
		case ButtonCode::Num_6: return "Num_6";
		case ButtonCode::Num_7: return "Num_7";
		case ButtonCode::Num_8: return "Num_8";
		case ButtonCode::Num_9: return "Num_9";
		case ButtonCode::Num_0: return "Num_0";
		case ButtonCode::Np_1:	return "Np_1";
		case ButtonCode::Np_2:	return "Np_2";
		case ButtonCode::Np_3:	return "Np_3";
		case ButtonCode::Np_4:	return "Np_4";
		case ButtonCode::Np_5:	return "Np_5";
		case ButtonCode::Np_6:	return "Np_6";
		case ButtonCode::Np_7:	return "Np_7";
		case ButtonCode::Np_8:	return "Np_8";
		case ButtonCode::Np_9:	return "Np_9";
		case ButtonCode::Np_0:	return "Np_0";
		case ButtonCode::Np_Add:		return "Np_Add";
		case ButtonCode::Np_Minus:		return "Np_Minus";
		case ButtonCode::Np_Multiply:	return "Np_Multiply";
		case ButtonCode::Np_Divide:		return "Np_Divide";
		case ButtonCode::Np_Enter:		return "Np_Enter";
		case ButtonCode::Np_Decimal:	return "Np_Decimal";
		case ButtonCode::Backspace:		return "Backspace";
		case ButtonCode::Tab:			return "Tab";
		case ButtonCode::Enter:			return "Enter";
		case ButtonCode::Pause:			return "Pause";
		case ButtonCode::Numlock:		return "Numlock";
		case ButtonCode::ScrollLock:	return "ScrollLock";
		case ButtonCode::Capslock:		return "Capslock";
		case ButtonCode::Escape:		return "Escape";
		case ButtonCode::Space:			return "Space";
		case ButtonCode::Insert:		return "Insert";
		case ButtonCode::Delete:		return "Delete";
		case ButtonCode::Home:			return "Home";
		case ButtonCode::End:			return "End";
		case ButtonCode::PageUp:		return "PageUp";
		case ButtonCode::PageDown:		return "PageDown";
		case ButtonCode::F1:			return "F1";
		case ButtonCode::F2:			return "F2";
		case ButtonCode::F3:			return "F3";
		case ButtonCode::F4:			return "F4";
		case ButtonCode::F5:			return "F5";
		case ButtonCode::F6:			return "F6";
		case ButtonCode::F7:			return "F7";
		case ButtonCode::F8:			return "F8";
		case ButtonCode::F9:			return "F9";
		case ButtonCode::F10:			return "F10";
		case ButtonCode::F11:			return "F11";
		case ButtonCode::F12:			return "F12";
		case ButtonCode::LCtrl:			return "LCtrl";
		case ButtonCode::RCtrl:			return "RCtrl";
		case ButtonCode::LShift:		return "LShift";
		case ButtonCode::RShift:		return "RShift";
		case ButtonCode::LAlt:			return "LAlt";
		case ButtonCode::RAlt:			return "RAlt";
		case ButtonCode::LGUI:			return "LGUI";
		case ButtonCode::RGUI:			return "RGUI";
		case ButtonCode::Semicolon:		return "Semicolon";
		case ButtonCode::Equals:		return "Equals";
		case ButtonCode::Comma:			return "Comma";
		case ButtonCode::Minus:			return "Minus";
		case ButtonCode::Period:		return "Period";
		case ButtonCode::Slash:			return "Slash";
		case ButtonCode::Backquote:		return "Backquote";
		case ButtonCode::LeftBracket:	return "LeftBracket";
		case ButtonCode::RightBracket:	return "RightBracket";
		case ButtonCode::Backslash:		return "Backslash";
		case ButtonCode::Quote:			return "Quote";
		case ButtonCode::Mouse_Left:	return "Mouse_Left";
		case ButtonCode::Mouse_Right:	return "Mouse_Right";
		case ButtonCode::Mouse_Middle:	return "Mouse_Middle";
		case ButtonCode::Mouse_X1:		return "Mouse_X1";
		case ButtonCode::Mouse_X2:		return "Mouse_X2";
		case ButtonCode::Gamepad_A:		return "Gamepad_A";
		case ButtonCode::Gamepad_B:		return "Gamepad_B";
		case ButtonCode::Gamepad_X:		return "Gamepad_X";
		case ButtonCode::Gamepad_Y:		return "Gamepad_Y";
		case ButtonCode::Gamepad_LB:	return "Gamepad_LB";
		case ButtonCode::Gamepad_RB:	return "Gamepad_RB";
		case ButtonCode::Gamepad_LS:	return "Gamepad_LS";
		case ButtonCode::Gamepad_RS:	return "Gamepad_RS";
		case ButtonCode::Gamepad_Dpad_Left:		return "Gamepad_Dpad_Left";
		case ButtonCode::Gamepad_Dpad_Up:		return "Gamepad_Dpad_Up";
		case ButtonCode::Gamepad_Dpad_Right:	return "Gamepad_Dpad_Right";
		case ButtonCode::Gamepad_Dpad_Down:		return "Gamepad_Dpad_Down";
		case ButtonCode::Gamepad_SpecialLeft:	return "Gamepad_SpecialLeft";
		case ButtonCode::Gamepad_SpecialRight:	return "Gamepad_SpecialRight";
		default: return "Unknown";
		}
	}

	std::string ToString(AxisCode _axis) {
		switch (_axis) {
		case AxisCode::Mouse_X:					return "Mouse_X";
		case AxisCode::Mouse_Y:					return "Mouse_Y";
		case AxisCode::Gamepad_LeftX:			return "Gamepad_LeftX";
		case AxisCode::Gamepad_LeftY:			return "Gamepad_LeftY";
		case AxisCode::Gamepad_RightX:			return "Gamepad_RightX";
		case AxisCode::Gamepad_RightY:			return "Gamepad_RightY";
		case AxisCode::Gamepad_LeftTrigger:		return "Gamepad_LeftTrigger";
		case AxisCode::Gamepad_RightTrigger:	return "Gamepad_RightTrigger";
		default: return "Unknown";
		}
	}
}
