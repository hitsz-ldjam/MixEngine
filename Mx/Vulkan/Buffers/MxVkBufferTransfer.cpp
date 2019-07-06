#include "MxVkBufferTransfer.h"
#include "MxVkBuffer.h"
#include "../Image/MxVkImage.h"

namespace Mix {
	namespace Graphics {

		DataToBuffer::DataToBuffer(const std::shared_ptr<DeviceAllocator>& _allocator,
								   const char* _src,
								   const std::shared_ptr<Buffer>& _dst,
								   vk::DeviceSize _dstOffset,
								   vk::DeviceSize _size)
			: mDst(_dst),
			mCopyRegion(0, _dstOffset, _size) {
			mSrc = std::make_shared<Buffer>(_allocator,
											vk::BufferUsageFlagBits::eTransferSrc,
											vk::MemoryPropertyFlagBits::eHostVisible |
											vk::MemoryPropertyFlagBits::eHostCoherent,
											_size);
			mSrc->uploadData(_src, 0, _size);
		}

		void DataToBuffer::operator()(const vk::CommandBuffer& _cmd) const {
			_cmd.copyBuffer(mSrc->get(), mDst->get(), mCopyRegion);
		}

		BufferToBuffer::BufferToBuffer(const std::shared_ptr<Buffer>& _src,
									   const std::shared_ptr<Buffer>& _dst,
									   const vk::DeviceSize& _srcOffset,
									   const vk::DeviceSize& _dstOffset,
									   const vk::DeviceSize& _size)
			: mSrc(_src), mDst(_dst), mCopyRegions(1, vk::BufferCopy(_srcOffset, _dstOffset, _size)) {
		}

		BufferToBuffer::BufferToBuffer(const std::shared_ptr<Buffer>& _src,
									   const std::shared_ptr<Buffer>& _dst,
									   const vk::BufferCopy& _region)
			: mSrc(_src), mDst(_dst), mCopyRegions(1, _region) {
		}

		BufferToBuffer::BufferToBuffer(const std::shared_ptr<Buffer>& _src,
									   const std::shared_ptr<Buffer>& _dst,
									   const std::vector<vk::BufferCopy>& _regions)
			: mSrc(_src), mDst(_dst), mCopyRegions(_regions) {
		}

		BufferToImage::BufferToImage(const std::shared_ptr<Buffer>& _src,
									 const std::shared_ptr<Image>& _dst,
									 const vk::BufferImageCopy& _region)
			: mSrc(_src), mDst(_dst), mCopyRegions(1, _region) {
		}

		BufferToImage::BufferToImage(const std::shared_ptr<Buffer>& _src,
									 const std::shared_ptr<Image>& _dst,
									 const std::vector<vk::BufferImageCopy>& _regions)
			: mSrc(_src), mDst(_dst), mCopyRegions(_regions) {
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
								 const std::shared_ptr<Image>& _dst,
								 const vk::BufferImageCopy& _region)
			: mDst(_dst), mCopyRegion(_region) {
			mSrc = std::make_shared<Buffer>(_allocator,
											vk::BufferUsageFlagBits::eTransferSrc,
											vk::MemoryPropertyFlagBits::eHostVisible |
											vk::MemoryPropertyFlagBits::eHostCoherent,
											_size,
											_src);
		}

		void DataToImage::operator()(const vk::CommandBuffer& _cmd) const {
			_cmd.copyBufferToImage(mSrc->get(),
								   mDst->get(),
								   vk::ImageLayout::eTransferDstOptimal,
								   mCopyRegion);
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
