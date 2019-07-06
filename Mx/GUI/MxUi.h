#pragma once
#ifndef	MX_IMGUI_H_
#define MX_IMGUI_H_

#include "../Engine/MxModuleBase.h"
#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>
#include "../Math/MxVector2.h"
#include "imgui/imgui.h"

namespace Mix {
	namespace Graphics {
		class CommandBufferHandle;
		class DescriptorSetLayout;
		class Buffer;
		class Vulkan;
	}

	namespace Resource {
		class Texture2D;
	}

	class Ui : public ModuleBase {
	public:
		void init();

		void update();

		void render();

		ImDrawData* getDrawData();

		Resource::Texture2D& getFontTexture() const { return *mFontTexture; }

		Graphics::Buffer& getVertexBuffer() const { return *mVertexBuffers[mCurrFrame]; }

		Graphics::Buffer& getIndexBuffer() const { return *mIndexBuffers[mCurrFrame]; }

		vk::IndexType getIndexType() const;

		Math::Vector2f getScale() const;

		Math::Vector2f getTranslate() const;

	private:
		static char* sClipboardTextData;

		static void SetClipboardText(void*, const char* _text);

		static const char* GetClipboardText(void*);

		void mapImGui();
		void createFontTexture();
		void updateExtent();
		void updateMouse();
		void updateMouseCursor();
		void updateKeyEvent();

		void checkBuffers(const size_t _vertex, const size_t _index);

		Graphics::Vulkan* mVulkan = nullptr;
		SDL_Cursor* mMouseCursors[ImGuiMouseCursor_COUNT] = { nullptr };

		size_t mCurrFrame = 0;
		std::shared_ptr<Resource::Texture2D> mFontTexture;
		std::vector<std::shared_ptr<Graphics::Buffer>> mVertexBuffers;
		std::vector<std::shared_ptr<Graphics::Buffer>> mIndexBuffers;

	};
}

#endif
