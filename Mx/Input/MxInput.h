#pragma once
#ifndef MX_INPUT_MODULE_H_
#define MX_INPUT_MODULE_H_

#include "../Engine/MxModuleBase.h"
#include "MxKeyCode.h"
#include "../Engine/MxPlatform.h"
#include "MxGamepad.h"
#include "MxKeyboard.h"
#include "MxMouse.h"

namespace Mix {
    class Input :public ModuleBase {
    public:
        enum ButtonState {
            ButtonState_Up = 0x01,
            ButtonState_Down = 0x02,
            ButtonState_Hold = 0x04
        };

        static Input* Get();

        ~Input();

        void load() override;
        void init() override {}

        // todo
        bool anyKeyDown();

        float getAxis(AxisCode _axis, int32_t _id = -1) const;
        Vector2f getGamepadLeftStickAxis(int32_t _id = -1) const;
        Vector2f getGamepadRightStickAxis(int32_t _id = -1) const;

        bool isButtonHold(ButtonCode _button, int32_t _id = -1) const;
        bool isButtonDown(ButtonCode _button, int32_t _id = -1) const;
        bool isButtonUp(ButtonCode _button, int32_t _id = -1) const;

        const Vector2i& getMouseWheelScrollDelta() const;
        const Vector2i& getMousePosition() const;
        const Vector2i& getMousePositionDelta() const;
        bool isMouseDoubleClick() const;

        void nextFrame();
    private:
        void onMouseButton(const PFMouseButtonEventData& _data);
        void onMouseMove(const PFMouseMoveEventData& _data);
        void onMouseWheel(const PFMouseWheelEventData& _data);
        void onKeyboard(const PFKeyboardEventData& _data);
        void onGamepadButton(const PFGamepadButtonEventData& _data);
        void onGamepadAxis(const PFGamepadAxisEventData& _data);
        void onGamepadDevice(const PFGamepadDeviceEventData& _data);

        Gamepad* getGamepadFromId(int32_t _id) const;
        uint8_t getButtonState(ButtonCode _button, int32_t _id = -1) const;

        std::unique_ptr<Keyboard> mKeyboard;
        std::unique_ptr<Mouse> mMouse;
        std::vector<Gamepad*> mGamepads;

        bool mAnyKeyDown = false;
        void onAnyKeyDown();
    };
}

#endif
