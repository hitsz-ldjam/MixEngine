#pragma once
#ifndef	MX_IMGUI_H_
#define MX_IMGUI_H_

#include "../Engine/MxModuleBase.h"
#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>
#include "../Math/MxVector2.h"
#include "../../ThirdPartyLibs/imgui/imgui.h"

namespace Mix {
	class Texture2D;

	namespace Vulkan {
		class CommandBufferHandle;
		class DescriptorSetLayout;
		class Buffer;
		class VulkanAPI;	
	}

	class Ui : public ModuleBase {
	public:
		static Ui* Get();

		void init();

		void render();

		ImDrawData* getDrawData();

		Texture2D& getFontTexture() const { return *mFontTexture; }

		Vulkan::Buffer& getVertexBuffer() const { return *mVertexBuffers[mCurrFrame]; }

		Vulkan::Buffer& getIndexBuffer() const { return *mIndexBuffers[mCurrFrame]; }

		vk::IndexType getIndexType() const;

		Vector2f getScale() const;

		Vector2f getTranslate() const;

		void update();

		/**
		 * \brief Process event from SDL
		 * \return true if Ui wants to intercept this event
		 */
		bool process(const SDL_Event& _event);

	private:
		void setImGui();
		void createFontTexture();
		void checkBuffers(const size_t _vertex, const size_t _index);

		Vulkan::VulkanAPI* mVulkan = nullptr;
		SDL_Cursor* mMouseCursors[ImGuiMouseCursor_COUNT] = { nullptr };

		size_t mCurrFrame = 0;
		std::shared_ptr<Texture2D> mFontTexture;
		std::vector<std::shared_ptr<Vulkan::Buffer>> mVertexBuffers;
		std::vector<std::shared_ptr<Vulkan::Buffer>> mIndexBuffers;

		static char* sClipboardTextData;
		static void SetClipboardText(void*, const char* _text);
		static const char* GetClipboardText(void*);

		void updateMouseCursor();

		bool processMouse(const SDL_Event& _event);
		bool processKey(const SDL_Event& _event);
	};
}

#endif
