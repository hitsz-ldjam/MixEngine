#pragma once
#ifndef MX_VK_BUFFER_TRANSFER_H_
#define MX_VK_BUFFER_TRANSFER_H_

#include "../Memory/MxVkAllocator.h"
#include "vulkan/vulkan.hpp"

namespace Mix {
	namespace Graphics {
		class Device;
		class Buffer;
		class Image;

		class DataToBuffer {
		public:
			DataToBuffer(const std::shared_ptr<DeviceAllocator>& _allocator,
						 const char* _src,
						 const std::shared_ptr<Buffer>& _dst,
						 vk::DeviceSize _dstOffset,
						 vk::DeviceSize _size);

			void operator()(const vk::CommandBuffer& _cmd) const;
		private:
			std::shared_ptr<Buffer> mSrc;
			std::shared_ptr<Buffer> mDst;
			vk::BufferCopy mCopyRegion;
		};

		class BufferToBuffer {
		public:
			BufferToBuffer(const std::shared_ptr<Buffer>& _src,
						   const std::shared_ptr<Buffer>& _dst,
						   const vk::DeviceSize& _srcOffset,
						   const vk::DeviceSize& _dstOffset,
						   const vk::DeviceSize& _size);

			BufferToBuffer(const std::shared_ptr<Buffer>& _src,
						   const std::shared_ptr<Buffer>& _dst,
						   const vk::BufferCopy& _region);

			BufferToBuffer(const std::shared_ptr<Buffer>& _src,
						   const std::shared_ptr<Buffer>& _dst,
						   const std::vector<vk::BufferCopy>& _regions);

			void operator()(const vk::CommandBuffer& _cmd) const;

		private:
			std::shared_ptr<Buffer> mSrc;
			std::shared_ptr<Buffer> mDst;
			std::vector<vk::BufferCopy> mCopyRegions;
		};

		class BufferToImage {
		public:
			BufferToImage(const std::shared_ptr<Buffer>& _src,
						  const std::shared_ptr<Image>& _dst,
						  const vk::BufferImageCopy& _region);

			BufferToImage(const std::shared_ptr<Buffer>& _src,
						  const std::shared_ptr<Image>& _dst,
						  const std::vector<vk::BufferImageCopy>& _regions);

			void operator()(const vk::CommandBuffer& _cmd) const;

		private:
			std::shared_ptr<Buffer> mSrc;
			std::shared_ptr<Image> mDst;
			std::vector<vk::BufferImageCopy> mCopyRegions;
		};

		class DataToImage {
		public:
			DataToImage(const std::shared_ptr<DeviceAllocator>& _allocator,
						const char* _src,
						const vk::DeviceSize& _size,
						const std::shared_ptr<Image>& _dst,
						const vk::BufferImageCopy& _region);

			void operator()(const vk::CommandBuffer& _cmd) const;
		private:
			std::shared_ptr<Buffer> mSrc;
			std::shared_ptr<Image> mDst;
			vk::BufferImageCopy mCopyRegion;
		};
	}
}

#endif
