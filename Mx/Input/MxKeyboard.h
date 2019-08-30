#pragma once
#ifndef MX_KEYBOARD_H_
#define MX_KEYBOARD_H_

#include "../Engine/MxPlantform.h"
#include <SDL2/SDL.h>

namespace Mix {
	class Input;

	class Keyboard :GeneralBase::NoCopyBase {
		friend class Input;
	public:
		explicit Keyboard(Input* _input);

		Keyboard(Keyboard&& _other) noexcept;

		Keyboard& operator=(Keyboard&& _other) noexcept;

		uint8_t getButtonState(ButtonCode _button);

		void nextFrame();

	private:
		static const uint32_t sButtonCount = static_cast<uint32_t>(ButtonCode::KeyboardCount);

		Input* mInput;
		uint8_t mButtonState[sButtonCount]{};

		void updateButton(const PFKeyboardEventData& _data);
	};
}

#endif
