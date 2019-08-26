#include "MxVkBuffer.h"
#include "../Device/MxVkDevice.h"
#include "../CommandBuffer/MxVkCommanddBufferHandle.h"
#include "../MxVulkan.h"
#include "../../Graphics/MxGraphics.h"

namespace Mix {
	namespace Vulkan {
		Buffer::Buffer(const std::shared_ptr<DeviceAllocator>& _allocator,
					   const vk::BufferUsageFlags& _usage,
					   const vk::MemoryPropertyFlags& _memoryProperty,
					   const vk::DeviceSize _size,
					   const void* _data,
					   const vk::SharingMode _sharingMode)
			: mAllocator(_allocator) {

			vk::BufferCreateInfo createInfo;
			createInfo.size = _size;
			createInfo.usage = _usage;
			createInfo.sharingMode = _sharingMode;

			mBuffer = _allocator->getDevice()->get().createBuffer(createInfo);
			vk::MemoryRequirements memRequirements;
			mMemory = _allocator->allocate(mBuffer, _memoryProperty, &memRequirements);

			mAlignment = memRequirements.alignment;
			mSize = _size;
			mUsages = _usage;
			mMemoryProperty = _memoryProperty;

			// copy data to the buffer
			if (_data) {
				setData(_data, _size);
			}
		}

		void Buffer::swap(Buffer& _other) noexcept {
			using std::swap;
			swap(mBuffer, _other.mBuffer);
			swap(mSize, _other.mSize);
			swap(mAlignment, _other.mAlignment);
			swap(mMemory, _other.mMemory);
			swap(mUsages, _other.mUsages);
			swap(mMemoryProperty, _other.mMemoryProperty);
			swap(mAllocator, _other.mAllocator);
		}

		Buffer::~Buffer() {
			if (mBuffer) {
				mAllocator->getDevice()->get().destroyBuffer(mBuffer);
				mAllocator->deallocate(mMemory);
			}
		}

		WriteDescriptorSet Buffer::getWriteDescriptor(uint32_t _binding, vk::DescriptorType _descriptorType, const std::optional<OffsetSize64>& _offsetSize) const {
			vk::DescriptorBufferInfo bufferInfo{ mBuffer,0,mSize };

			if (_offsetSize) {
				bufferInfo.offset = _offsetSize.value().offset;
				bufferInfo.range = _offsetSize.value().size;
			}

			vk::WriteDescriptorSet write{ nullptr,_binding,0,1,_descriptorType };
			return WriteDescriptorSet(write, bufferInfo);
		}

		void Buffer::setData(const void * _data, const vk::DeviceSize& _size) {
			setData(_data, 0, _size);
		}

		void Buffer::setData(const void* _data, const OffsetSize64& _offsetSize) {
			setData(_data, _offsetSize.offset, _offsetSize.size);
		}

		void Buffer::setData(const std::vector<char>& _data, const vk::DeviceSize& _offset) {
			setData(_data.data(), _offset, _data.size());
		}

		void Buffer::setData(const void* _data, const vk::DeviceSize& _dstOffset, const vk::DeviceSize& _size) const {
			assert(_data && "Null pointer.");
			assert((_dstOffset + _size) <= mSize && "Out of range");

			if (mMemoryProperty & vk::MemoryPropertyFlagBits::eDeviceLocal &&
				!(mMemoryProperty & vk::MemoryPropertyFlagBits::eHostVisible)) {
				// buffer can't be accessed by CPU

				Buffer staging = Buffer(mAllocator, vk::BufferUsageFlagBits::eTransferSrc,
										vk::MemoryPropertyFlagBits::eHostCoherent |
										vk::MemoryPropertyFlagBits::eHostVisible,
										_size, _data);

				CommandBufferHandle cmd{ Graphics::Get()->getRenderApi().getTransferCommandPool() };
				cmd.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
				vk::BufferCopy copy{ 0,_dstOffset,_size };
				cmd.get().copyBuffer(staging.get(), mBuffer, copy);
				cmd.end();
				cmd.submit();
				cmd.wait();
			}
			else {
				memcpy(static_cast<char*>(mMemory.ptr) + _dstOffset, _data, _size);
				//if not HOST_COHERENT then need to be flushed
				if (!(mMemoryProperty & vk::MemoryPropertyFlagBits::eHostCoherent))
					flush();
			}
		}

		std::vector<char> Buffer::getData(const OffsetSize64& _offsetSize) {
			assert((_offsetSize.offset + _offsetSize.size) <= mSize && "Out of range");

			if (mMemoryProperty & vk::MemoryPropertyFlagBits::eDeviceLocal &&
				!(mMemoryProperty & vk::MemoryPropertyFlagBits::eHostVisible)) {
				// buffer can't be accessed by CPU

				Buffer staging = Buffer(mAllocator, vk::BufferUsageFlagBits::eTransferDst,
										vk::MemoryPropertyFlagBits::eHostCoherent |
										vk::MemoryPropertyFlagBits::eHostVisible,
										_offsetSize.size);

				CommandBufferHandle cmd{ Graphics::Get()->getRenderApi().getTransferCommandPool() };
				cmd.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
				vk::BufferCopy copy{ 0,_offsetSize.offset,_offsetSize.size };
				cmd.get().copyBuffer(mBuffer, staging.get(), copy);
				cmd.end();
				cmd.submit();
				cmd.wait();

				return staging.getData({ 0,_offsetSize.size });
			}
			else {
				std::vector<char> result;
				result.reserve(_offsetSize.size);
				memcpy(result.data(), static_cast<char*>(mMemory.ptr) + _offsetSize.offset, _offsetSize.size);
				return result;
			}
		}

		std::vector<char> Buffer::getData() {
			return getData({ 0, mSize });
		}

		void Buffer::flush(const vk::DeviceSize _size, const vk::DeviceSize _offset) const {
			vk::MappedMemoryRange mappedRange;
			mappedRange.memory = mMemory.memory;
			mappedRange.offset = _offset;
			mappedRange.size = _size;
			mAllocator->getDevice()->get().flushMappedMemoryRanges(mappedRange);
		}

		void Buffer::invalidate(const vk::DeviceSize _size, const vk::DeviceSize _offset) const {
			vk::MappedMemoryRange mappedRange = {};
			mappedRange.memory = mMemory.memory;
			mappedRange.offset = _offset;
			mappedRange.size = _size;
			mAllocator->getDevice()->get().invalidateMappedMemoryRanges(mappedRange);
		}
	}
}
