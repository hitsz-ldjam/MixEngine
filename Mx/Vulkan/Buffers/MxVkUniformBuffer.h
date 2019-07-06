#pragma once
#ifndef MX_VK_UNIFORMBUFFER_H_
#define MX_VK_UNIFORMBUFFER_H_

#include "../Core/MxVkCore.h"
#include "MxVkBuffer.h"
#include "../Descriptor/MxVkDescriptor.h"
#include "../../Utils/MxOffsetSize.hpp"

namespace Mix {
	namespace Graphics {
		class DynamicUniformBuffer {
		public:
			explicit DynamicUniformBuffer(const std::shared_ptr<DeviceAllocator>& _allocator,
										  const uint32_t _sizePerObj,
										  const uint32_t _count);

			void reset() {
				mCurrCount = 0;
			}

			void next() {
				++mCurrCount;
			}

			WriteDescriptorSet getWriteDescriptorSet(const vk::DescriptorSet& _set,
			                                         const uint32_t _binding,
			                                         std::optional<Utils::OffsetSize<uint32_t>> _offsetSize = std::nullopt) const;

			uint32_t getOffset() const { return mUniformSize * mCurrCount; }

			void pushBack(const void* _data, const vk::DeviceSize& _size) const { mBuffer->uploadData(_data, getOffset(), _size); }

			bool full() const { return mCurrCount >= mMaxUniformCount; }

		private:
			std::unique_ptr<Buffer> mBuffer;

			uint32_t mUniformSize;
			uint32_t mMaxUniformCount;

			uint32_t mCurrCount;
		};

	};
}

#endif
