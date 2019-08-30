#include "MxKeyboard.h"
#include "MxInputModule.h"
#include "../Log/MxLog.h"

namespace Mix {

	Keyboard::Keyboard(Input* _input) :mInput(_input) {
	}

	Keyboard::Keyboard(Keyboard&& _other) noexcept : mInput(_other.mInput) {
		std::copy(std::begin(_other.mButtonState), std::end(_other.mButtonState), mButtonState);

		_other.mInput = nullptr;
	}

	Keyboard& Keyboard::operator=(Keyboard&& _other) noexcept {
		mInput = _other.mInput;
		std::copy(std::begin(_other.mButtonState), std::end(_other.mButtonState), mButtonState);

		_other.mInput = nullptr;

		return *this;
	}

	uint8_t Keyboard::getButtonState(ButtonCode _button) {
		return mButtonState[MX_BC_KEYBOARD_INDEX(_button)];
	}

	void Keyboard::nextFrame() {
		for (uint32_t i = 0; i < sButtonCount; ++i) {
			mButtonState[i] &= ~(Input::ButtonState_Up | Input::ButtonState_Down);
		}
	}

	void Keyboard::updateButton(const PFKeyboardEventData& _data) {
		uint32_t index = MX_BC_KEYBOARD_INDEX(_data.key);
		if (_data.state & PFButtonState_Pressed && !(mButtonState[index] & Input::ButtonState_Hold)) {
			mButtonState[index] = Input::ButtonState_Down | Input::ButtonState_Hold;
		}
		else if (_data.state & PFButtonState_Released) {
			mButtonState[index] = Input::ButtonState_Up;
		}
	}
}
