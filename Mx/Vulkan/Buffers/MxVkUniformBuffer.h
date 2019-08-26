#pragma once
#ifndef MX_VK_UNIFORMBUFFER_H_
#define MX_VK_UNIFORMBUFFER_H_

#include "MxVkBuffer.h"

namespace Mix {
	namespace Vulkan {
		class UniformBuffer :public Buffer {
			explicit UniformBuffer(const std::shared_ptr<DeviceAllocator>& _allocator, uint32_t _size);
		};

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

			WriteDescriptorSet getWriteDescriptor(const uint32_t _binding) const;

			uint32_t getOffset() const { return mUniformSize * mCurrCount; }

			void pushBack(const void* _data, const vk::DeviceSize& _size) const { mBuffer->setData(_data, getOffset(), _size); }

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
