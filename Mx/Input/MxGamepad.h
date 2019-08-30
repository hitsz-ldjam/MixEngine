#pragma once
#ifndef MX_GAMEPAD_H_
#define MX_GAMEPAD_H_

#include "../Engine/MxPlantform.h"
#include <SDL2/SDL.h>

namespace Mix {
	class Gamepad :GeneralBase::NoCopyBase {
		friend class Input;

	public:
		Gamepad(int _index, Input* _input);

		~Gamepad();

		Gamepad(Gamepad&& _other) noexcept;

		Gamepad& operator=(Gamepad&& _other) noexcept;

		const std::string& getName() const { return mName; }

		int getInstanceId() const { return mInstanceId; }

		uint8_t getButtonState(ButtonCode _button) const;

		float getAxis(AxisCode _axis) const;

		Math::Vector2f getAxis(AxisCode _x, AxisCode _y) const;

		void nextFrame();

	private:
		static const uint32_t sAxisCount = static_cast<uint32_t>(AxisCode::GamepadAxisCount);
		static const uint32_t sButtonCount = static_cast<uint32_t>(ButtonCode::GamepadCount);

		int mInstanceId = -1;
		std::string mName;
		SDL_GameController* mGameController;
		Input* mInput;

		float mStickDeadZone = 0.2f;
		float mAxisState[sAxisCount]{};
		uint8_t mButtonState[sButtonCount]{};

		void updateAxis(const PFGamepadAxisEventData& _data);
		void updateButton(const PFGamepadButtonEventData& _data);
	};
}

#endif
