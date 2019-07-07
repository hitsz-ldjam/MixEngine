#pragma once
#ifndef MX_VK_DESCRIPTOR_H_
#define MX_VK_DESCRIPTOR_H_

#include "../Device/MxVkDevice.h"
#include <map>
#include "../../Utils/MxArrayProxy.h"

namespace Mix {
	namespace Graphics {
		class  WriteDescriptorSet {
		public:
			WriteDescriptorSet(const vk::WriteDescriptorSet &_writeDescriptorSet, const vk::DescriptorImageInfo &_imageInfo) :
				mWriteDescriptorSet(_writeDescriptorSet),
				mImageInfo(std::make_unique<vk::DescriptorImageInfo>(_imageInfo)),
				mBufferInfo(nullptr) {
				mWriteDescriptorSet.pImageInfo = mImageInfo.get();
			}

			WriteDescriptorSet(const vk::WriteDescriptorSet &_writeDescriptorSet, const vk::DescriptorBufferInfo &_bufferInfo) :
				mWriteDescriptorSet(_writeDescriptorSet),
				mImageInfo(nullptr),
				mBufferInfo(std::make_unique<vk::DescriptorBufferInfo>(_bufferInfo)) {
				mWriteDescriptorSet.pBufferInfo = mBufferInfo.get();
			}

			const VkWriteDescriptorSet &get() const { return mWriteDescriptorSet; }

		private:
			vk::WriteDescriptorSet mWriteDescriptorSet;
			std::unique_ptr<vk::DescriptorImageInfo> mImageInfo;
			std::unique_ptr<vk::DescriptorBufferInfo> mBufferInfo;
		};

		class DescriptorSetLayout : public GeneralBase::NoCopyBase {
		public:
			explicit DescriptorSetLayout(const std::shared_ptr<Device>& _device)
				:mDevice(_device), mBindings(new std::vector<vk::DescriptorSetLayoutBinding>()) {
			}

			void addBindings(const uint32_t _binding,
							 const vk::DescriptorType _type,
							 const uint32_t _count,
							 const vk::ShaderStageFlags _stage,
							 const vk::Sampler* _immutableSamplers = nullptr);

			void addBindings(ArrayProxy<const vk::DescriptorSetLayoutBinding> _bindings);

			void create();

			const vk::DescriptorSetLayout& get() const { return *mLayout; }

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			const std::vector<vk::DescriptorSetLayoutBinding>& getBindings() const { return *mBindings; }

		private:
			std::shared_ptr<Device> mDevice;
			vk::UniqueDescriptorSetLayout mLayout;
			std::shared_ptr<std::vector<vk::DescriptorSetLayoutBinding>> mBindings;
		};



		class DescriptorPool :public GeneralBase::NoCopyBase {
		public:
			~DescriptorPool();

			explicit DescriptorPool(const std::shared_ptr<Device>& _device) :mDevice(_device) {}

			DescriptorPool(DescriptorPool&& _other) noexcept { swap(_other); }

			DescriptorPool& operator=(DescriptorPool&& _other) noexcept { swap(_other); return *this; }

			void swap(DescriptorPool& _other) noexcept;

			void create(uint32_t _maxSets);

			void addPoolSize(vk::DescriptorType _type, uint32_t _count);

			const vk::DescriptorPool& get() const { return mDescriptorPool; }

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			std::vector<vk::DescriptorSet> allocDescriptorSet(const std::vector<vk::DescriptorSetLayout>& _layouts) const;

			std::vector<vk::DescriptorSet> allocDescriptorSet(const vk::DescriptorSetLayout _layout, const uint32_t _count) const;

			vk::DescriptorSet allocDescriptorSet(const vk::DescriptorSetLayout _layout) const;

			std::vector<vk::DescriptorSet> allocDescriptorSet(const std::vector<DescriptorSetLayout>& _layouts) const;

			std::vector<vk::DescriptorSet> allocDescriptorSet(const DescriptorSetLayout& _layout, const uint32_t _count) const;

			vk::DescriptorSet allocDescriptorSet(const DescriptorSetLayout& _layout) const;

			void dealloc(const vk::DescriptorSet& _set) const;

			void dealloc(const std::vector<vk::DescriptorSet>& _sets) const;

		private:
			vk::DescriptorPool mDescriptorPool;
			std::shared_ptr<Device> mDevice;
			std::map<vk::DescriptorType, uint32_t> mPoolSizes;
		};
	}
}
#endif // !MX_VK_DESCRIPTOR_H_
