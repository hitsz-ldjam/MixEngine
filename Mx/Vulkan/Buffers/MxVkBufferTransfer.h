#pragma once
#ifndef MX_VK_BUFFER_TRANSFER_H_
#define MX_VK_BUFFER_TRANSFER_H_

#include "../Memory/MxVkAllocator.h"
#include "vulkan/vulkan.hpp"

namespace Mix {
	namespace Vulkan {
		class Device;
		class Buffer;
		class Image;

		class DataToBuffer {
		public:
			DataToBuffer(const std::shared_ptr<DeviceAllocator>& _allocator,
						 const char* _src,
						 std::shared_ptr<Buffer> _dst,
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
			BufferToBuffer(std::shared_ptr<Buffer> _src,
						   std::shared_ptr<Buffer> _dst,
						   const vk::DeviceSize& _srcOffset,
						   const vk::DeviceSize& _dstOffset,
						   const vk::DeviceSize& _size);

			BufferToBuffer(std::shared_ptr<Buffer> _src,
						   std::shared_ptr<Buffer> _dst,
						   const vk::BufferCopy& _region);

			BufferToBuffer(std::shared_ptr<Buffer> _src,
						   std::shared_ptr<Buffer> _dst,
						   const std::vector<vk::BufferCopy>& _regions);

			void operator()(const vk::CommandBuffer& _cmd) const;

			const std::vector<vk::BufferCopy>& getCopyRegions() const { return mCopyRegions; }
		private:
			std::shared_ptr<Buffer> mSrc;
			std::shared_ptr<Buffer> mDst;
			std::vector<vk::BufferCopy> mCopyRegions;
		};

		class BufferToImage {
		public:
			BufferToImage(std::shared_ptr<Buffer> _src,
						  std::shared_ptr<Image> _dst,
						  const vk::BufferImageCopy& _region);

			BufferToImage(std::shared_ptr<Buffer> _src,
						  std::shared_ptr<Image> _dst,
						  const std::vector<vk::BufferImageCopy>& _regions);

			void operator()(const vk::CommandBuffer& _cmd) const;

			const std::vector<vk::BufferImageCopy>& getCopyRegions() const { return mCopyRegions; }
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
						std::shared_ptr<Image> _dst,
						const vk::ImageSubresourceLayers& _imageSubresource,
						const vk::Offset3D& _imageOffset,
						const vk::Extent3D& _imageExtent);

			void operator()(const vk::CommandBuffer& _cmd) const;
		private:
			std::shared_ptr<Buffer> mSrc;
			std::shared_ptr<Image> mDst;
			vk::BufferImageCopy mCopyRegion;
		};

		class ImageToImage {
		public:
			ImageToImage(const std::shared_ptr<DeviceAllocator>& _allocator,
						 std::shared_ptr<Image> _src,
						 std::shared_ptr<Image> _dst,
						 std::vector<vk::ImageCopy> _regions);

			void operator()(const vk::CommandBuffer& _cmd) const;

			const std::vector<vk::ImageCopy>& getCopyRegions() const { return mCopyRegions; }
		private:
			std::shared_ptr<Image> mSrc;
			std::shared_ptr<Image> mDst;
			std::vector<vk::ImageCopy> mCopyRegions;
		};
	}
}

#endif
