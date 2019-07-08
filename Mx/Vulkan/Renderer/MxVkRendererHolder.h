#pragma once
#ifndef MX_VK_RENDERER_HOLDER_H_
#define MX_VK_RENDERER_HOLDER_H_

#include "../../Utils/MxGeneralBase.hpp"
#include "MxVkRendererBase.h"
#include <unordered_map>

namespace Mix {
	namespace Graphics {
		class Vulkan;

		class RendererHolder : GeneralBase::NoCopyBase {
		public:
			RendererHolder() = default;

			RendererHolder(RendererHolder&& _other) noexcept = default;

			RendererHolder& operator=(RendererHolder&& _other) noexcept = default;

			uint32_t addRenderer(RendererBase* _renderer);

			RendererBase* getRenderer(const uint32_t _index);

			void removeRenderer(const uint32_t _index);

			bool hasRenderer(const uint32_t _index) const { return mRenderers.count(_index); }

			void clear();

			uint32_t rendererCount() const { return static_cast<uint32_t>(mRenderers.size()); }

		private:
			std::unordered_map<uint32_t, RendererBase*> mRenderers;
			uint32_t mCurrIndex = 1;
		};
	}
}

#endif
