#include "../../ThirdPartyLibs/imgui/imgui.h"
#include "MxGUi.h"
#include "../../MixEngine.h"
#include "../Vulkan/CommandBuffer/MxVkCommandPool.h"
#include "../Graphics/Texture/MxTexture.h"
#include "../Vulkan/Descriptor/MxVkDescriptorSet.h"
#include "../Input/MxInput.h"
#include "../Vulkan/Buffers/MxVkBuffer.h"
#include "../Vulkan/Swapchain/MxVkSwapchain.h"
#include "../Graphics/MxGraphics.h"
#include "../Vulkan/MxVulkan.h"
#include "../Math/MxColor.h"
#include <stb_image/stb_image_write.h>


namespace Mix {

    GUI* GUI::Get() {
        return MixEngine::Instance().getModule<GUI>();
    }

    GUI::~GUI() {
        int a = 1;
    }

    void GUI::load() {
    }

    void GUI::init() {
        using namespace std::placeholders;
        Platform::MouseButtonEvent.connect(std::bind(&GUI::onMouseButtonEvent, this, _1));
        Platform::MouseWheelEvent.connect(std::bind(&GUI::onMouseWheelEvent, this, _1));
        Platform::MouseMoveEvent.connect(std::bind(&GUI::onMouseMoveEvent, this, _1));
        Platform::KeyboardEvent.connect(std::bind(&GUI::onKeyboardEvent, this, _1));

        mVulkan = &Graphics::Get()->getRenderApi();
        if (!mVulkan) {
            Log::Error("Imgui need Vulkan as a dependency");
            return;
        }

        ImGui::CreateContext();
        ImGui::StyleColorsLight();

        setImGui();
        createFontTexture();
        //mVertexBuffers.resize(mVulkan->getSwapchain()->imageCount());
        //mIndexBuffers.resize(mVulkan->getSwapchain()->imageCount());
    }

    void GUI::render() {
        if (!GUIEvent.empty())
            GUIEvent.trigger();

    }

    bool GUI::getRenderData(UIRenderData& _renderData) {
        auto drawData = ImGui::GetDrawData();

        if (!drawData)
            return false;
        mCurrFrame = mVulkan->getSwapchain()->getCurrFrame();
        const auto fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
        const auto fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
        if (fbWidth <= 0 || fbHeight <= 0 || drawData->TotalVtxCount == 0)
            return false;

        _renderData.drawData = drawData;
        _renderData.fontTexture = mFontTexture;
        _renderData.fontTextureChanged = mFontTextureChanged;
        _renderData.indexFormat = getIndexFormat();
        _renderData.scale = Vector2f(2.0f / drawData->DisplaySize.x, 2.0f / drawData->DisplaySize.y);
        _renderData.translate = { -1.0f - drawData->DisplayPos.x * _renderData.scale[0],-1.0f - _renderData.drawData->DisplayPos.y * _renderData.scale[1] };

        return true;
    }

    void GUI::beginGUI() {
        ImGui::NewFrame();
    }

    void GUI::endGUI() {
        render();
        ImGui::Render();
        ImGui::EndFrame();
    }

    /*void UI::checkBuffers(const size_t _vertex, const size_t _index) {
        if (!mVertexBuffers[mCurrFrame] || mVertexBuffers[mCurrFrame]->size() < _vertex)
            mVertexBuffers[mCurrFrame] = std::make_shared<Vulkan::Buffer>(mVulkan->getAllocator(),
                                                                          vk::BufferUsageFlagBits::eVertexBuffer,
                                                                          vk::MemoryPropertyFlagBits::eHostVisible |
                                                                          vk::MemoryPropertyFlagBits::eHostCoherent,
                                                                          _vertex);
        if (!mIndexBuffers[mCurrFrame] || mIndexBuffers[mCurrFrame]->size() < _index)
            mIndexBuffers[mCurrFrame] = std::make_shared<Vulkan::Buffer>(mVulkan->getAllocator(),
                                                                         vk::BufferUsageFlagBits::eIndexBuffer,
                                                                         vk::MemoryPropertyFlagBits::eHostVisible |
                                                                         vk::MemoryPropertyFlagBits::eHostCoherent,
                                                                         _index);

    }*/

    char* GUI::sClipboardTextData = nullptr;

    IndexFormat GUI::getIndexFormat() const {
        return sizeof(ImDrawIdx) == 2 ? IndexFormat::UInt16 : IndexFormat::UInt32;
    }

    void GUI::SetClipboardText(void*, const char* _text) {
        SDL_SetClipboardText(_text);
    }

    const char* GUI::GetClipboardText(void*) {
        if (sClipboardTextData)
            SDL_free(static_cast<void*>(sClipboardTextData));
        sClipboardTextData = SDL_GetClipboardText();
        return sClipboardTextData;
    }

    void GUI::setImGui() {
        auto& io = ImGui::GetIO();

        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.MouseDrawCursor = false;

        io.BackendPlatformName = "MixEngine ImGui";

        // map key to SDl
        io.KeyMap[ImGuiKey_Tab] = int(ButtonCode::Tab);
        io.KeyMap[ImGuiKey_LeftArrow] = int(ButtonCode::Left);
        io.KeyMap[ImGuiKey_RightArrow] = int(ButtonCode::Right);
        io.KeyMap[ImGuiKey_UpArrow] = int(ButtonCode::Up);
        io.KeyMap[ImGuiKey_DownArrow] = int(ButtonCode::Down);
        io.KeyMap[ImGuiKey_PageUp] = int(ButtonCode::PageUp);
        io.KeyMap[ImGuiKey_PageDown] = int(ButtonCode::PageDown);
        io.KeyMap[ImGuiKey_Home] = int(ButtonCode::Home);
        io.KeyMap[ImGuiKey_End] = int(ButtonCode::End);
        io.KeyMap[ImGuiKey_Insert] = int(ButtonCode::Insert);
        io.KeyMap[ImGuiKey_Delete] = int(ButtonCode::Delete);
        io.KeyMap[ImGuiKey_Backspace] = int(ButtonCode::Backspace);
        io.KeyMap[ImGuiKey_Space] = int(ButtonCode::Space);
        io.KeyMap[ImGuiKey_Enter] = int(ButtonCode::Enter);
        io.KeyMap[ImGuiKey_Escape] = int(ButtonCode::Escape);
        io.KeyMap[ImGuiKey_A] = int(ButtonCode::A);
        io.KeyMap[ImGuiKey_C] = int(ButtonCode::C);
        io.KeyMap[ImGuiKey_V] = int(ButtonCode::V);
        io.KeyMap[ImGuiKey_X] = int(ButtonCode::X);
        io.KeyMap[ImGuiKey_Y] = int(ButtonCode::Y);
        io.KeyMap[ImGuiKey_Z] = int(ButtonCode::Z);

        // handle clipboard
        io.SetClipboardTextFn = SetClipboardText;
        io.GetClipboardTextFn = GetClipboardText;

        // map mouse shape
        mMouseCursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        mMouseCursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
        mMouseCursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
        mMouseCursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
        mMouseCursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
        mMouseCursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
        mMouseCursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
        mMouseCursors[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

        // size
        auto extent = Window::Get()->getExtent();
        auto drawableExtent = Window::Get()->getDrawableSize();
        io.DisplaySize = ImVec2(static_cast<float>(extent.x), static_cast<float>(extent.y));
        if (extent.x > 0 && extent.y > 0)
            io.DisplayFramebufferScale = ImVec2(static_cast<float>(drawableExtent.x) / extent.x,
                                                static_cast<float>(drawableExtent.y) / extent.y);
    }

    void GUI::createFontTexture() {
        unsigned char* pixels;
        int width, height;

        auto& io = ImGui::GetIO();
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        size_t size = width * height * 4 * sizeof(char);

        mFontTexture = std::make_shared<Texture2D>(width, height, TextureFormat::R8G8B8A8_Unorm);
        mFontTexture->setPixels(reinterpret_cast<const char*>(pixels), size);
        mFontTexture->apply(false);
    }

    void GUI::update() {
        updateMouseCursor();
    }

    void GUI::updateMouseCursor() {
        auto& io = ImGui::GetIO();

        if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
            return;

        ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
        if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None) {
            SDL_ShowCursor(SDL_FALSE);
        }
        else {
            // Show OS mouse cursor
            // SDL_SetCursor(mMouseCursors[cursor] ? mMouseCursors[cursor] : mMouseCursors[ImGuiMouseCursor_Arrow]);
            // SDL_ShowCursor(SDL_TRUE);
        }

        if (io.WantSetMousePos) {
            Platform::SetCursorPositionGlobal({ io.MousePos.x, io.MousePos.y });
        }
    }

    /*bool UI::process(const SDL_Event& _event) {
        auto& io = ImGui::GetIO();
        bool result = false;

        switch (_event.type) {
        case SDL_MOUSEWHEEL:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEMOTION:
            result = processMouse(_event);
            break;
        case SDL_TEXTINPUT:
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            result = processKey(_event);
            break;
        case SDL_WINDOWEVENT_SHOWN:
        case SDL_WINDOWEVENT_RESIZED:
            auto extent = Window::Get()->getExtent();
            auto drawableExtent = Window::Get()->getDrawableSize();
            io.DisplaySize = ImVec2(static_cast<float>(extent.x), static_cast<float>(extent.y));
            if (extent.x > 0 && extent.y > 0)
                io.DisplayFramebufferScale = ImVec2(static_cast<float>(drawableExtent.x) / extent.x,
                                                    static_cast<float>(drawableExtent.y) / extent.y);
            break;
        }

        if (io.WantSetMousePos) {
            SDL_WarpMouseInWindow(Window::Get()->rawPtr(),
                                  static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y));
            auto mousePos = Input::Get()->getMousePosition();
            io.MousePos.x = static_cast<float>(mousePos.x);
            io.MousePos.y = static_cast<float>(mousePos.y);
        }

        return result;
    }*/

    /*bool UI::processMouse(const SDL_Event& _event) {
        auto& io = ImGui::GetIO();
        switch (_event.type) {
        case SDL_MOUSEWHEEL:
            if (_event.wheel.x > 0) io.MouseWheelH += 1;
            if (_event.wheel.x < 0) io.MouseWheelH -= 1;
            if (_event.wheel.y > 0) io.MouseWheel += 1;
            if (_event.wheel.y < 0) io.MouseWheel -= 1;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (_event.button.button == SDL_BUTTON_LEFT) io.MouseDown[0] = true;
            if (_event.button.button == SDL_BUTTON_RIGHT) io.MouseDown[1] = true;
            if (_event.button.button == SDL_BUTTON_MIDDLE) io.MouseDown[2] = true;
            break;
        case SDL_MOUSEBUTTONUP:
            if (_event.button.button == SDL_BUTTON_LEFT) io.MouseDown[0] = false;
            if (_event.button.button == SDL_BUTTON_RIGHT) io.MouseDown[1] = false;
            if (_event.button.button == SDL_BUTTON_MIDDLE) io.MouseDown[2] = false;
            break;
        case SDL_MOUSEMOTION:
            io.MousePos.x = static_cast<float>(_event.motion.x);
            io.MousePos.y = static_cast<float>(_event.motion.y);
            break;
        }

        return io.WantCaptureMouse;
    }*/

    /*bool UI::processKey(const SDL_Event& _event) {
        auto& io = ImGui::GetIO();
        switch (_event.type) {
        case SDL_TEXTINPUT:
            io.AddInputCharactersUTF8(_event.text.text);
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            int key = _event.key.keysym.scancode;
            IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
            io.KeysDown[key] = (_event.type == SDL_KEYDOWN);
            io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
            io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
            io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
            io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
            break;
        }

        return io.WantCaptureKeyboard;
    }*/

    void GUI::onMouseMoveEvent(const PFMouseMoveEventData& _data) {
        auto& io = ImGui::GetIO();
        io.MousePos.x = static_cast<float>(_data.pos.x);
        io.MousePos.y = static_cast<float>(_data.pos.y);
    }

    void GUI::onMouseButtonEvent(const PFMouseButtonEventData& _data) {
        auto& io = ImGui::GetIO();
        if (_data.state & PFButtonState_Pressed) {
            if (_data.button == ButtonCode::Mouse_Left)io.MouseDown[0] = true;
            if (_data.button == ButtonCode::Mouse_Right) io.MouseDown[1] = true;
            if (_data.button == ButtonCode::Mouse_Middle) io.MouseDown[2] = true;
        }

        if (_data.state & PFButtonState_Released) {
            if (_data.button == ButtonCode::Mouse_Left)io.MouseDown[0] = false;
            if (_data.button == ButtonCode::Mouse_Right) io.MouseDown[1] = false;
            if (_data.button == ButtonCode::Mouse_Middle) io.MouseDown[2] = false;
        }
    }

    void GUI::onMouseWheelEvent(const PFMouseWheelEventData& _data) {
        auto& io = ImGui::GetIO();
        if (_data.xy.x > 0) io.MouseWheelH += 1;
        else if (_data.xy.y < 0) io.MouseWheelH -= 1;

        if (_data.xy.x > 0) io.MouseWheel += 1;
        else    if (_data.xy.y < 0) io.MouseWheel -= 1;
    }

    void GUI::onKeyboardEvent(const PFKeyboardEventData& _data) {
        auto& io = ImGui::GetIO();
        int key = static_cast<int>(_data.key);
        MX_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
        io.KeysDown[key] = _data.state & PFButtonState_Pressed;
        io.KeyShift = _data.mode & PFMode_LShift || _data.mode & PFMode_RShift;
        io.KeyCtrl = _data.mode & PFMode_LCtrl || _data.mode & PFMode_RCtrl;
        io.KeyAlt = _data.mode & PFMode_LAlt || _data.mode & PFMode_RAlt;
    }
}
