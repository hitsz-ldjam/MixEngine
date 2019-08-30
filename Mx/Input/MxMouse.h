#pragma once
#ifndef MX_MOUSE_H_
#define MX_MOUSE_H_

#include "../Engine/MxPlantform.h"
#include "SDL2/SDL.h"

namespace Mix {
	class Mouse :GeneralBase::NoCopyBase {
		friend class Input;

	public:
		explicit Mouse(Input* _input);

		Mouse(Mouse&& _other) noexcept;

		Mouse& operator=(Mouse&& _other) noexcept;

		uint8_t getButtonState(ButtonCode _button);

		const Math::Vector2i& getPosState() const { return mPosState; }

		const Math::Vector2i& getPosDeltaState() const { return mDeltaPosState; }

		const Math::Vector2i& getMouseWheelState() const { return mWheelState; }

		float getAxis(AxisCode _axis);

		void nextFrame();

	private:
		static const uint32_t sButtonCount = static_cast<uint32_t>(ButtonCode::MouseCount);

		Input* mInput;

		Math::Vector2i mPosState;
		Math::Vector2i mDeltaPosState;
		Math::Vector2i mWheelState;
		uint8_t mButtonState[sButtonCount]{};

		void updatePos(const PFMouseMoveEventData& _data);
		void updateButton(const PFMouseButtonEventData& _data);
		void updateWheel(const PFMouseWheelEventData& _data);
		void pressedButton(ButtonCode _button);
		void releasedButton(ButtonCode _button);
	};
}

#endif
