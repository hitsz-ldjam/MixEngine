#include "../../ThirdPartyLibs/imgui/imgui.h"
#include "MxUi.h"
#include "../../MixEngine.h"
#include "../Vulkan/CommandBuffer/MxVkCommandPool.h"
#include "../Graphics/Texture/MxTexture.h"
#include "../Vulkan/Descriptor/MxVkDescriptorSet.h"
#include "../Input/MxInput.h"
#include "../Vulkan/Buffers/MxVkBuffer.h"
#include "../Vulkan/Swapchain/MxVkSwapchain.h"
#include "../Graphics/MxGraphics.h"
#include "../Vulkan/MxVulkan.h"


namespace Mix {

	Ui* Ui::Get() {
		return MixEngine::Instance().getModule<Ui>();
	}

	void Ui::init() {
		mVulkan = &Graphics::Get()->getRenderApi();
		if (!mVulkan) {
			Log::Error("Imgui need Vulkan as a dependency");
			return;
		}

		ImGui::CreateContext();
		ImGui::StyleColorsLight();

		setImGui();
		createFontTexture();
		mVertexBuffers.resize(mVulkan->getSwapchain()->imageCount());
		mIndexBuffers.resize(mVulkan->getSwapchain()->imageCount());
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
			mVertexBuffers[mCurrFrame]->setData(cmdList->VtxBuffer.Data, vOffset, cmdList->VtxBuffer.size_in_bytes());
			mIndexBuffers[mCurrFrame]->setData(cmdList->IdxBuffer.Data, iOffset, cmdList->IdxBuffer.size_in_bytes());
			vOffset += cmdList->VtxBuffer.size_in_bytes();
			iOffset += cmdList->IdxBuffer.size_in_bytes();
		}
	}

	ImDrawData* Ui::getDrawData() {
		return ImGui::GetDrawData();
	}

	void Ui::checkBuffers(const size_t _vertex, const size_t _index) {
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

	void Ui::setImGui() {
		auto& io = ImGui::GetIO();

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.MouseDrawCursor = true;

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

		// size
		auto extent = Window::Get()->extent();
		auto drawableExtent = Window::Get()->drawableSize();
		io.DisplaySize = ImVec2(static_cast<float>(extent.x), static_cast<float>(extent.y));
		if (extent.x > 0 && extent.y > 0)
			io.DisplayFramebufferScale = ImVec2(static_cast<float>(drawableExtent.x) / extent.x,
												static_cast<float>(drawableExtent.y) / extent.y);
	}

	void Ui::createFontTexture() {
		unsigned char* pixels;
		int width, height;

		auto& io = ImGui::GetIO();
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		size_t size = width * height * 4 * sizeof(char);

		mFontTexture = std::make_shared<Texture2D>(width, height, TextureFormat::R8G8B8A8_Unorm);
		mFontTexture->setPixels(reinterpret_cast<const char*>(pixels), size);
		mFontTexture->apply(false);
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
		return { -1.0f - drawData->DisplayPos.x * scale[0],-1.0f - drawData->DisplayPos.y * scale[1] };
	}

	void Ui::update() {
		updateMouseCursor();
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

	bool Ui::process(const SDL_Event& _event) {
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
			auto extent = Window::Get()->extent();
			auto drawableExtent = Window::Get()->drawableSize();
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
	}

	bool Ui::processMouse(const SDL_Event& _event) {
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
	}

	bool Ui::processKey(const SDL_Event& _event) {
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
	}
}
