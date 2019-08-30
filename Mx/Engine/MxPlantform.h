#pragma once
#ifndef MX_PLANTFORM_H_
#define MX_PLANTFORM_H_

#include "../Utils/MxGeneralBase.hpp"
#include "../Math/MxVector.h"
#include "../Utils/MxEvent.h"
#include "../Input/MxKeyCode.h"

namespace Mix {
	enum PFMode {
		PFMode_None = 0x0000,

		PFMode_LCtrl = 0x0001,
		PFMode_RCtrl = 0x0002,

		PFMode_LShift = 0x0004,
		PFMode_RShift = 0x0008,

		PFMode_LAlt = 0x0010,
		PFMode_RAlt = 0x0020,

		PFMode_Ctrl = PFMode_LCtrl | PFMode_RCtrl,
		PFMode_Shift = PFMode_LShift | PFMode_RShift,
		PfMode_Alt = PFMode_LAlt | PFMode_RAlt
	};

	enum PFMouseButton {
		PFMouseButton_None = 0x00,
		PFMouseButton_Left = 0x01,
		PFMouseButton_Right = 0x02,
		PFMouseButton_Middle = 0x04,
		PFMouseButton_X1 = 0x08,
		PFMouseButton_X2 = 0x10
	};

	enum PFGamepadAxis {
		PFGamepad_Axis_None = 0x00,
		PFGamepad_Axis_LeftX = 0x01,
		PFGamepad_Axis_LeftY = 0x02,
		PFGamepad_Axis_RightX = 0x04,
		PFGamepad_Axis_RightY = 0x08,
		PFGamepad_Axis_LeftTrigger = 0x10,
		PFGamepad_Axis_RightTrigger = 0x20
	};

	enum PFButtonState {
		PFButtonState_Released = 0x01,
		PFButtonState_Pressed = 0x02
	};

	enum PFDeviceState {
		PFDeviceState_Added = 0x01,
		PFDeviceState_Removed = 0x02
	};

	struct PFMouseMoveEventData {
		Math::Vector2i pos;
		Math::Vector2i relativePos;
		uint8_t state = 0; /**< Test with PFMpuseButton */
	};

	struct PFMouseButtonEventData {
		uint8_t state = 0; /**< Test with PFButtonState */
		uint8_t clicks = 0;
		ButtonCode button; /**< Test with PFMouseButton, only one bit will be set */
		Math::Vector2i pos;
	};

	struct PFMouseWheelEventData {
		Math::Vector2i xy;
	};

	struct PFKeyboardEventData {
		uint8_t state = 0; /**< Test with PFButtonState */
		uint16_t mode = 0; /**< Test with PFMode to check modifier state */
		ButtonCode key; /**< Only the keyboard-related portion of ButtonCode will be included */
	};

	struct PFGamepadButtonEventData {
		int32_t id = -1;
		uint8_t state = 0; /**< Test with PFButtonState */
		ButtonCode button = ButtonCode::Unknown;
	};

	struct PFGamepadAxisEventData {
		int32_t id = -1;
		AxisCode axis;
		float value = 0; /*< Range: [-1, 1] */
	};

	struct PFGamepadDeviceEventData {
		uint8_t deviceState = 0;
		int32_t which = 0;
	};

	class Plantform :public GeneralBase::StaticBase {
	public:
		static bool Initialize();

		static void ShutDown();

		static void SetCursorPositionGlobal(const Math::Vector2i& _screenPos);

		static void HideCursor();

		static void ShowCursor();

		static bool IsCursorHidden();

		static bool CopyToClipboard(const std::string& _string);

		static bool HasClipText();

		static void SetRelativeMouseMode(bool _mode);

		static bool GetRelativeMouseMode();

		static const std::string CopyFromClipboard();

		static void Update();

		// Events
		static Event<void(const PFMouseMoveEventData&)> MouseMoveEvent;

		static Event<void(const PFMouseButtonEventData&)> MouseButtonEvent;

		static Event<void(const PFMouseWheelEventData&)> MouseWheelEvent;

		static Event<void(const PFKeyboardEventData&)> KeyboardEvent;

		static Event<void(const PFGamepadAxisEventData&)> GamepadAxisEvent;

		static Event<void(const PFGamepadButtonEventData&)> GamepadButtonEvent;

		static Event<void(const PFGamepadDeviceEventData&)> GamepadDeviceEvent;

		static Event<void()> RequireQuitEvent;
	private:
		static void PollEvent();

		static bool sInitialized;
		static bool sCursorVisible;
		static bool sRelativeMouseMode;
	};
}

#endif
