#include "MxVkRendererHolder.h"
#include "../../Log/MxLog.h"

namespace Mix {
	namespace Graphics {
		uint32_t RendererHolder::addRenderer(RendererBase* _renderer) {
			mRenderers.emplace(mCurrIndex, _renderer);
			return mCurrIndex++;
		}

		RendererBase * RendererHolder::getRenderer(const uint32_t _index) {
			if (mRenderers.count(_index) == 0) {
				Debug::Log::Error("No requested Renderer");
				return nullptr;
			}
			return mRenderers[_index];
		}

		void RendererHolder::removeRenderer(const uint32_t _index) {
			mRenderers.erase(_index);
		}

		void RendererHolder::clear() {
			for (auto& pair : mRenderers) {
				delete pair.second;
			}
		}
	}
}
