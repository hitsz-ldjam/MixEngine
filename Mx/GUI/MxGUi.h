#pragma once
#ifndef	MX_IMGUI_H_
#define MX_IMGUI_H_

#include "../Engine/MxModuleBase.h"
#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>
#include "../Math/MxVector2.h"
#include "../../ThirdPartyLibs/imgui/imgui.h"
#include "../Definitions/MxCommonType.h"
#include "../Input/MxMouse.h"

namespace Mix {
    class Texture2D;

    namespace Vulkan {
        class CommandBufferHandle;
        class DescriptorSetLayout;
        class Buffer;
        class VulkanAPI;
    }

    class GUI : public ModuleBase {
    public:
        struct UIRenderData {
            ImDrawData* drawData;
            IndexFormat indexFormat;
            std::shared_ptr<Texture2D> fontTexture;
            bool fontTextureChanged = true;
            Vector2f scale;
            Vector2f translate;
        };

        static GUI* Get();

        ~GUI();

        void load() override;

        void init() override;

        void update();

        bool getRenderData(UIRenderData& _renderData);

        void beginGUI();

        void endGUI();

        Event<void()> GUIEvent;

    private:
        void setImGui();

        void render();

        void createFontTexture();

        IndexFormat getIndexFormat() const;

        Vulkan::VulkanAPI* mVulkan = nullptr;
        SDL_Cursor* mMouseCursors[ImGuiMouseCursor_COUNT] = { nullptr };

        size_t mCurrFrame = 0;
        bool mFontTextureChanged = true;
        std::shared_ptr<Texture2D> mFontTexture;

        static char* sClipboardTextData;
        static void SetClipboardText(void*, const char* _text);
        static const char* GetClipboardText(void*);

        void updateMouseCursor();

        void onMouseMoveEvent(const PFMouseMoveEventData& _data);

        void onMouseButtonEvent(const PFMouseButtonEventData& _data);

        void onMouseWheelEvent(const PFMouseWheelEventData& _data);

        void onKeyboardEvent(const PFKeyboardEventData& _data);
    };
}

#endif
