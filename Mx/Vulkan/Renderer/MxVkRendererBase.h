#pragma once
#ifndef MX_VK_RENDERER_BASE_H_
#define MX_VK_RENDERER_BASE_H_

#include "../../Definitions/MxGeneralBase.hpp"

namespace Mix {
	namespace Graphics {
		class Vulkan;
		class CommandBufferHandle;

		class RendererBase : public GeneralBase::NoCopyBase {
		public:
			virtual ~RendererBase() = default;

			void setVulkanPointer(Vulkan* _vulkan) { mVulkan = _vulkan; }

			virtual void init() = 0;

			virtual void update(const uint32_t _currFrame) = 0;

			virtual void render(CommandBufferHandle& _cmd, const uint32_t _currFrame) = 0;

		protected:
			Vulkan* mVulkan = nullptr;
		};
	}
}

#endif
