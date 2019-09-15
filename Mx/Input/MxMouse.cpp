#include "MxMouse.h"
#include "MxInput.h"

namespace Mix {
    Mouse::Mouse(Input* _input) :mInput(_input) {
    }

    Mouse::Mouse(Mouse&& _other) noexcept :mInput(_other.mInput) {
        mPosState = _other.mPosState;
        mDeltaPosState = _other.mDeltaPosState;
        std::copy(std::begin(_other.mButtonState), std::end(_other.mButtonState), mButtonState);

        _other.mInput = nullptr;
    }

    Mouse& Mouse::operator=(Mouse&& _other) noexcept {
        mInput = _other.mInput;

        mPosState = _other.mPosState;
        mDeltaPosState = _other.mDeltaPosState;
        std::copy(std::begin(_other.mButtonState), std::end(_other.mButtonState), mButtonState);

        _other.mInput = nullptr;

        return *this;
    }

    uint8_t Mouse::getButtonState(ButtonCode _button) {
        return mButtonState[MX_BC_MOUSE_INDEX(_button)];
    }

    float Mouse::getAxis(AxisCode _axis) {
        switch (_axis) {
        case AxisCode::Mouse_X:     return mPosState.x;
        case AxisCode::Mouse_Y:     return mPosState.x;
        case AxisCode::Mouse_WheelX:    return mWheelState.x;
        case AxisCode::Mouse_WheelY:    return mWheelState.y;
        default:return 0.0f;
        }
    }

    void Mouse::nextFrame() {
        for (uint32_t i = 0; i < sButtonCount; ++i) {
            mButtonState[i] &= ~(Input::ButtonState_Up | Input::ButtonState_Down);
        }

        mDeltaPosState = Vector2i::Zero;
        mWheelState = Vector2i::Zero;
    }

    void Mouse::updatePos(const PFMouseMoveEventData& _data) {
        mPosState = _data.pos;
        mDeltaPosState = _data.relativePos;

        if (_data.state != 0) {
            if (_data.state & PFMouseButton_Left)	pressedButton(ButtonCode::Mouse_Left);
            if (_data.state & PFMouseButton_Right)	pressedButton(ButtonCode::Mouse_Right);
            if (_data.state & PFMouseButton_Middle)	pressedButton(ButtonCode::Mouse_Middle);
            if (_data.state & PFMouseButton_X1)		pressedButton(ButtonCode::Mouse_X1);
            if (_data.state & PFMouseButton_X2)		pressedButton(ButtonCode::Mouse_X2);
        }
    }

    void Mouse::updateButton(const PFMouseButtonEventData& _data) {
        uint32_t index = MX_BC_MOUSE_INDEX(_data.button);
        if (_data.state & PFButtonState_Pressed) {
            mButtonState[index] = Input::ButtonState_Down | Input::ButtonState_Hold;
        }
        else if (_data.state & PFButtonState_Released) {
            mButtonState[index] = Input::ButtonState_Up;
        }
    }

    void Mouse::updateWheel(const PFMouseWheelEventData& _data) {
        mWheelState = _data.xy;
    }

    void Mouse::pressedButton(ButtonCode _button) {
        mButtonState[MX_BC_MOUSE_INDEX(_button)] = Input::ButtonState_Down | Input::ButtonState_Hold;
    }

    void Mouse::releasedButton(ButtonCode _button) {
        mButtonState[MX_BC_MOUSE_INDEX(_button)] = Input::ButtonState_Up;
    }

}
