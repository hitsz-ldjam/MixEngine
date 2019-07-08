#include "imgui/imgui.h"
#include "MxUi.h"
#include "../../MixEngine.h"
#include "../Vulkan/MxVkGraphics.h"
#include "../Vulkan/CommandBuffer/MxVkCommandPool.h"
#include "../Resource/Texture/MxTexture.h"
#include "../Vulkan/Descriptor/MxVkDescriptor.h"
#include "../Input/MxInput.h"
#include "../Vulkan/Buffers/MxVkBuffer.h"
#include "../Vulkan/Swapchain/MxVkSwapchain.h"

namespace Mix {

	void Ui::init() {
		mVulkan = MixEngine::Instance().getModule<Graphics::Vulkan>();
		if (!mVulkan) {
			Debug::Log::Error("Imgui need Vulkan as a dependency");
			return;
		}

		ImGui::CreateContext();
		ImGui::StyleColorsLight();

		mapImGui();
		createFontTexture();
		mVertexBuffers.resize(mVulkan->getSwapchain()->imageCount());
		mIndexBuffers.resize(mVulkan->getSwapchain()->imageCount());
	}

	void Ui::update() {
		updateExtent();
		updateMouse();
		updateMouseCursor();
		updateKeyEvent();
	}

	void Ui::render() {
		mCurrFrame = mVulkan->getSwapchain()->currFrame();
		ImGui::NewFrame();
		bool open = true;
		ImGui::ShowDemoWindow(&open);
		ImGui::Render();

		const auto drawData = ImGui::GetDrawData();
		const auto fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
		const auto fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
		if (fbWidth <= 0 || fbHeight <= 0 || drawData->TotalVtxCount == 0)
			return;

		size_t vertexSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
		size_t indexSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);
		checkBuffers(vertexSize, indexSize);

		size_t vOffset = 0;
		size_t iOffset = 0;

		for (int i = 0; i < drawData->CmdListsCount; ++i) {
			auto cmdList = drawData->CmdLists[i];
			mVertexBuffers[mCurrFrame]->uploadData(cmdList->VtxBuffer.Data, vOffset, cmdList->VtxBuffer.size_in_bytes());
			mIndexBuffers[mCurrFrame]->uploadData(cmdList->IdxBuffer.Data, iOffset, cmdList->IdxBuffer.size_in_bytes());
			vOffset += cmdList->VtxBuffer.size_in_bytes();
			iOffset += cmdList->IdxBuffer.size_in_bytes();
		}
	}

	ImDrawData* Ui::getDrawData() {
		return ImGui::GetDrawData();
	}

	void Ui::updateExtent() {
		auto& io = ImGui::GetIO();
		assert(io.Fonts->IsBuilt());

		auto extent = MixEngine::Instance().getWindow().extent();
		auto drawableExtent = MixEngine::Instance().getWindow().drawableSize();
		io.DisplaySize = ImVec2(static_cast<float>(extent.x), static_cast<float>(extent.y));
		if (extent.x > 0 && extent.y > 0)
			io.DisplayFramebufferScale = ImVec2(static_cast<float>(drawableExtent.x) / extent.x,
												static_cast<float>(drawableExtent.y) / extent.y);
	}

	void Ui::updateMouse() {
		auto& io = ImGui::GetIO();

		if (io.WantSetMousePos)
			SDL_WarpMouseInWindow(MixEngine::Instance().getWindow().window(),
								  static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y));
		{
			auto mousePos = Input::MousePosition();
			io.MousePos.x = static_cast<float>(mousePos.x);
			io.MousePos.y = static_cast<float>(mousePos.y);
		}

		{
			io.MouseDown[0] = Input::MouseButtonState() & SDL_BUTTON_LMASK;
			io.MouseDown[1] = Input::MouseButtonState() & SDL_BUTTON_RMASK;
			io.MouseDown[2] = Input::MouseButtonState() & SDL_BUTTON_MMASK;
		}

		{
			auto scroll = Input::MouseScrollDelta();
			if (scroll.x > 0) io.MouseWheelH += 1;
			if (scroll.x < 0) io.MouseWheelH -= 1;
			if (scroll.y > 0) io.MouseWheel += 1;
			if (scroll.y < 0) io.MouseWheel -= 1;
		}

		io.KeyCtrl = Input::GetKeyDown(SDL_SCANCODE_LCTRL) || Input::GetKeyDown(SDL_SCANCODE_RCTRL);
		io.KeyShift = Input::GetKeyDown(SDL_SCANCODE_LSHIFT) || Input::GetKeyDown(SDL_SCANCODE_RSHIFT);
		io.KeyAlt = Input::GetKeyDown(SDL_SCANCODE_LALT) || Input::GetKeyDown(SDL_SCANCODE_RALT);
		io.KeySuper = Input::GetKeyDown(SDL_SCANCODE_LGUI);

		// ImVec2      MousePos;                       // Mouse position, in pixels. Set to ImVec2(-FLT_MAX,-FLT_MAX) if mouse is unavailable (on another screen, etc.)
		// bool        MouseDown[5];                   // Mouse buttons: 0=left, 1=right, 2=middle + extras. ImGui itself mostly only uses left button (BeginPopupContext** are using right button). Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
		// float       MouseWheel;                     // Mouse wheel Vertical: 1 unit scrolls about 5 lines text.
		// float       MouseWheelH;                    // Mouse wheel Horizontal. Most users don't have a mouse with an horizontal wheel, may not be filled by all back-ends.
		// bool        KeyCtrl;                        // Keyboard modifier pressed: Control
		// bool        KeyShift;                       // Keyboard modifier pressed: Shift
		// bool        KeyAlt;                         // Keyboard modifier pressed: Alt
		// bool        KeySuper;                       // Keyboard modifier pressed: Cmd/Super/Windows
		// bool        KeysDown[512];                  // Keyboard keys that are pressed (ideally left in the "native" order your engine has access to keyboard keys, so you can use your own defines/enums for keys).
		// float       NavInputs[ImGuiNavInput_COUNT]; // Gamepad inputs. Cleared back to zero by EndFrame(). Keyboard keys will be auto-mapped and be written here by NewFrame().


	}

	void Ui::updateMouseCursor() {
		auto& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
			return;

		ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
		if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None) {
			SDL_ShowCursor(SDL_FALSE);
		}
		else {
			// Show OS mouse cursor
			SDL_SetCursor(mMouseCursors[cursor] ? mMouseCursors[cursor] : mMouseCursors[ImGuiMouseCursor_Arrow]);
			SDL_ShowCursor(SDL_TRUE);
		}
	}

	void Ui::updateKeyEvent() {
		auto& io = ImGui::GetIO();


	}

	void Ui::checkBuffers(const size_t _vertex, const size_t _index) {
		if (!mVertexBuffers[mCurrFrame] || mVertexBuffers[mCurrFrame]->size() < _vertex)
			mVertexBuffers[mCurrFrame] = std::make_shared<Graphics::Buffer>(mVulkan->getAllocator(),
																			vk::BufferUsageFlagBits::eVertexBuffer,
																			vk::MemoryPropertyFlagBits::eHostVisible |
																			vk::MemoryPropertyFlagBits::eHostCoherent,
																			_vertex);
		if (!mIndexBuffers[mCurrFrame] || mIndexBuffers[mCurrFrame]->size() < _index)
			mIndexBuffers[mCurrFrame] = std::make_shared<Graphics::Buffer>(mVulkan->getAllocator(),
																		   vk::BufferUsageFlagBits::eIndexBuffer,
																		   vk::MemoryPropertyFlagBits::eHostVisible |
																		   vk::MemoryPropertyFlagBits::eHostCoherent,
																		   _index);

	}

	char* Ui::sClipboardTextData = nullptr;

	vk::IndexType Ui::getIndexType() const {
		return sizeof(ImDrawIdx) == 2 ? vk::IndexType::eUint16 : vk::IndexType::eUint32;
	}

	void Ui::SetClipboardText(void*, const char* _text) {
		SDL_SetClipboardText(_text);
	}

	const char* Ui::GetClipboardText(void*) {
		if (sClipboardTextData)
			SDL_free(static_cast<void*>(sClipboardTextData));
		sClipboardTextData = SDL_GetClipboardText();
		return sClipboardTextData;
	}

	void Ui::mapImGui() {
		auto& io = ImGui::GetIO();

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		io.BackendPlatformName = "MixEngine ImGui";

		// map key to SDl
		io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
		io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
		io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
		io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
		io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
		io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
		io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
		io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
		io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
		io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
		io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
		io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
		io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
		io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
		io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

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
	}

	void Ui::createFontTexture() {
		unsigned char* pixels;
		int width, height;

		auto& io = ImGui::GetIO();
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		size_t size = width * height * 4 * sizeof(char);

		mFontTexture = std::make_shared<Resource::Texture2D>(width,
															 height,
															 vk::Format::eR8G8B8A8Unorm,
															 size,
															 reinterpret_cast<char*>(pixels));
	}

	Math::Vector2f Ui::getScale() const {
		return ImGui::GetDrawData() ?
			Math::Vector2f(2.0f / ImGui::GetDrawData()->DisplaySize.x, 2.0f / ImGui::GetDrawData()->DisplaySize.y)
			: Math::Vector2f();
	}

	Math::Vector2f Ui::getTranslate() const {
		auto drawData = ImGui::GetDrawData();
		if (!drawData)
			return Math::Vector2f::Zero; // (0, 0);

		auto scale = getScale();
		return { -1.0f - drawData->DisplayPos.x * scale[0],
		-1.0f - drawData->DisplayPos.y * scale[1] };
	}
}
