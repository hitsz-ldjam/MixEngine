#include "MxGamepad.h"
#include "MxInput.h"

namespace Mix {
	Gamepad::Gamepad(int _index, Input* _input) {
		mGameController = SDL_GameControllerOpen(_index);
		if (!mGameController)
			throw Exception("Failed to open Gamepad [%1%]", _index);
		mName = SDL_GameControllerName(mGameController);
		mInstanceId = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(mGameController));
	}

	Gamepad::~Gamepad() {
		if (mGameController)
			SDL_GameControllerClose(mGameController);
	}

	Gamepad::Gamepad(Gamepad&& _other) noexcept
		: mInstanceId(_other.mInstanceId),
		mName(std::move(_other.mName)),
		mGameController(_other.mGameController),
		mInput(_other.mInput) {

		std::copy(std::begin(_other.mAxisState), std::end(_other.mAxisState), mAxisState);
		std::copy(std::begin(_other.mButtonState), std::end(_other.mButtonState), mButtonState);

		_other.mGameController = nullptr;
		_other.mInstanceId = -1;
	}

	Gamepad& Gamepad::operator=(Gamepad&& _other) noexcept {
		mInstanceId = _other.mInstanceId;
		mGameController = _other.mGameController;
		mInput = _other.mInput;

		std::copy(std::begin(_other.mAxisState), std::end(_other.mAxisState), mAxisState);
		std::copy(std::begin(_other.mButtonState), std::end(_other.mButtonState), mButtonState);

		_other.mInstanceId = -1;
		_other.mGameController = nullptr;
		return *this;
	}

	void Gamepad::updateAxis(const PFGamepadAxisEventData& _data) {
		if (std::abs(_data.value) > mStickDeadZone)
			mAxisState[MX_AC_GAMEPAD_INDEX(_data.axis)] = _data.value;
		else
			mAxisState[MX_AC_GAMEPAD_INDEX(_data.axis)] = 0.0f;
	}

	void Gamepad::updateButton(const PFGamepadButtonEventData& _data) {
		uint32_t index = MX_BC_GAMEPAD_INDEX(_data.button);
		if (_data.state & PFButtonState_Pressed) {
			mButtonState[index] = Input::ButtonState_Down | Input::ButtonState_Hold;
		}
		else if (_data.state & PFButtonState_Released) {
			mButtonState[index] = Input::ButtonState_Up;
		}
	}

	uint8_t Gamepad::getButtonState(ButtonCode _button) const {
		return mButtonState[MX_BC_GAMEPAD_INDEX(_button)];
	}

	float Gamepad::getAxis(AxisCode _axis) const {
		return mAxisState[MX_AC_GAMEPAD_INDEX(_axis)];
	}

	Vector2f Gamepad::getAxis(AxisCode _x, AxisCode _y) const {
		return {
			mAxisState[MX_AC_GAMEPAD_INDEX(_x)],
			mAxisState[MX_AC_GAMEPAD_INDEX(_y)]
		};
	}

	void Gamepad::nextFrame() {
		for (uint32_t i = 0; i < sButtonCount; ++i) {
			mButtonState[i] &= ~(Input::ButtonState_Up | Input::ButtonState_Down);
		}
	}
}
