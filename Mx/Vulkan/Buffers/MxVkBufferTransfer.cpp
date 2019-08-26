#include "MxVkBufferTransfer.h"
#include "MxVkBuffer.h"
#include "../Image/MxVkImage.h"

namespace Mix {
	namespace Vulkan {

		DataToBuffer::DataToBuffer(const std::shared_ptr<DeviceAllocator>& _allocator,
								   const char* _src,
								   std::shared_ptr<Buffer> _dst,
								   vk::DeviceSize _dstOffset,
								   vk::DeviceSize _size)
			: mDst(std::move(_dst)),
			mCopyRegion(0, _dstOffset, _size) {
			mSrc = std::make_shared<Buffer>(_allocator,
											vk::BufferUsageFlagBits::eTransferSrc,
											vk::MemoryPropertyFlagBits::eHostVisible |
											vk::MemoryPropertyFlagBits::eHostCoherent,
											_size);
			mSrc->setData(_src, 0, _size);
		}

		void DataToBuffer::operator()(const vk::CommandBuffer& _cmd) const {
			_cmd.copyBuffer(mSrc->get(), mDst->get(), mCopyRegion);
		}

		BufferToBuffer::BufferToBuffer(std::shared_ptr<Buffer> _src,
									   std::shared_ptr<Buffer> _dst,
									   const vk::DeviceSize& _srcOffset,
									   const vk::DeviceSize& _dstOffset,
									   const vk::DeviceSize& _size)
			: mSrc(std::move(_src)), mDst(std::move(_dst)), mCopyRegions(1, vk::BufferCopy(_srcOffset, _dstOffset, _size)) {
		}

		BufferToBuffer::BufferToBuffer(std::shared_ptr<Buffer> _src,
									   std::shared_ptr<Buffer> _dst,
									   const vk::BufferCopy& _region)
			: mSrc(std::move(_src)), mDst(std::move(_dst)), mCopyRegions(1, _region) {
		}

		BufferToBuffer::BufferToBuffer(std::shared_ptr<Buffer> _src,
									   std::shared_ptr<Buffer> _dst,
									   const std::vector<vk::BufferCopy>& _regions)
			: mSrc(std::move(_src)), mDst(std::move(_dst)), mCopyRegions(_regions) {
		}

		BufferToImage::BufferToImage(std::shared_ptr<Buffer> _src,
									 std::shared_ptr<Image> _dst,
									 const vk::BufferImageCopy& _region)
			: mSrc(std::move(_src)), mDst(std::move(_dst)), mCopyRegions(1, _region) {
		}

		BufferToImage::BufferToImage(std::shared_ptr<Buffer> _src,
									 std::shared_ptr<Image> _dst,
									 const std::vector<vk::BufferImageCopy>& _regions)
			: mSrc(std::move(_src)), mDst(std::move(_dst)), mCopyRegions(_regions) {
		}

		void BufferToBuffer::operator()(const vk::CommandBuffer& _cmd) const {
			_cmd.copyBuffer(mSrc->get(), mDst->get(), mCopyRegions);
		}

		void BufferToImage::operator()(const vk::CommandBuffer& _cmd) const {
			_cmd.copyBufferToImage(mSrc->get(), mDst->get(), vk::ImageLayout::eTransferDstOptimal, mCopyRegions);
		}

		DataToImage::DataToImage(const std::shared_ptr<DeviceAllocator>& _allocator,
								 const char* _src,
								 const vk::DeviceSize& _size,
								 std::shared_ptr<Image> _dst,
								 const vk::ImageSubresourceLayers& _imageSubresource,
								 const vk::Offset3D& _imageOffset,
								 const vk::Extent3D& _imageExtent) : mDst(std::move(_dst)) {
			mSrc = std::make_shared<Buffer>(_allocator,
											vk::BufferUsageFlagBits::eTransferSrc,
											vk::MemoryPropertyFlagBits::eHostVisible |
											vk::MemoryPropertyFlagBits::eHostCoherent,
											_size,
											_src);

			mCopyRegion.imageSubresource = _imageSubresource;
			mCopyRegion.imageOffset = _imageOffset;
			mCopyRegion.imageExtent = _imageExtent;
		}

		void DataToImage::operator()(const vk::CommandBuffer& _cmd) const {
			_cmd.copyBufferToImage(mSrc->get(),
								   mDst->get(),
								   vk::ImageLayout::eTransferDstOptimal,
								   mCopyRegion);
		}

		ImageToImage::ImageToImage(const std::shared_ptr<DeviceAllocator>& _allocator,
								   std::shared_ptr<Image> _src,
								   std::shared_ptr<Image> _dst,
								   std::vector<vk::ImageCopy> _regions)
			:mSrc(std::move(_src)), mDst(std::move(_dst)), mCopyRegions(std::move(_regions)) {
		}

		void ImageToImage::operator()(const vk::CommandBuffer& _cmd) const {
			_cmd.copyImage(mSrc->get(), vk::ImageLayout::eTransferSrcOptimal,
						   mDst->get(), vk::ImageLayout::eTransferDstOptimal,
						   mCopyRegions);
		}

		/*const vk::DeviceSize size = mMemory.size < _size ? mMemory.size : _size;

		if (!mMemory.ptr) {
			auto transBuffer = Buffer::CreateBuffer(mCore, mAllocator,
													vk::BufferUsageFlagBits::eTransferSrc,
													vk::MemoryPropertyFlagBits::eHostVisible |
													vk::MemoryPropertyFlagBits::eHostCoherent,
													size);

			auto fence = mCore->getSyncObjMgr().createFence();
			mCore->getDevice().resetFences(fence.get());

			transBuffer->copyTo(_data, size);

			vk::BufferImageCopy bufferCopyRegion;
			bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			bufferCopyRegion.imageSubresource.mipLevel = 0;
			bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent = mExtent;
			bufferCopyRegion.bufferOffset = 0;

			vk::ImageSubresourceRange subresourceRange;
			subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;


			TransferVkImageLayout(_cmd,
								  mImage,
								  vk::ImageLayout::eUndefined,
								  vk::ImageLayout::eTransferDstOptimal,
								  subresourceRange,
								  vk::PipelineStageFlagBits::eTopOfPipe,
								  vk::PipelineStageFlagBits::eTransfer);

			_cmd.copyBufferToImage(transBuffer->mBuffer,
								   mImage,
								   vk::ImageLayout::eTransferDstOptimal,
								   bufferCopyRegion);

			TransferVkImageLayout(_cmd,
								  mImage,
								  vk::ImageLayout::eTransferDstOptimal,
								  vk::ImageLayout::eShaderReadOnlyOptimal,
								  subresourceRange,
								  vk::PipelineStageFlagBits::eTransfer,
								  vk::PipelineStageFlagBits::eFragmentShader);
		}
		else
			memcpy(mMemory.ptr, _data, size);*/
	}
}
