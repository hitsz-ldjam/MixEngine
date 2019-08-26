#pragma once
#ifndef MX_VK_DESCRIPTOR_SET_H_
#define MX_VK_DESCRIPTOR_SET_H_

#include "../Device/MxVkDevice.h"
#include "../../Utils/MxArrayProxy.h"
#include "../CommandBuffer/MxVkCommanddBufferHandle.h"
#include "../../Utils/MxOffsetSize.hpp"
#include "../../Log/MxLog.h"

#include <map>
#include <utility>
#include "MxVkDescriptor.h"


namespace Mix {
	namespace Vulkan {
		class Descriptor;

		class DescriptorSetLayout {
		public:
			explicit DescriptorSetLayout(std::shared_ptr<Device> _device) :mDevice(std::move(_device)) {
			}

			DescriptorSetLayout(const DescriptorSetLayout& _other);

			DescriptorSetLayout(DescriptorSetLayout&& _other) noexcept;

			DescriptorSetLayout& operator=(DescriptorSetLayout _other);

			DescriptorSetLayout& operator=(DescriptorSetLayout&& _other) noexcept;

			void swap(DescriptorSetLayout& _other) noexcept;

			~DescriptorSetLayout();

			void addBinding(const vk::DescriptorSetLayoutBinding& _binding);

			void setBindings(std::initializer_list<vk::DescriptorSetLayoutBinding> _bindings);

			void create();

			const vk::DescriptorSetLayout& get() const { return mDescriptorSetLayout; }

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			std::vector<vk::DescriptorSetLayoutBinding> getBindings() const;

			std::optional<vk::DescriptorSetLayoutBinding> getBinding(uint32_t _binding) const;

			bool hasBinding(uint32_t _binding) const;

		private:
			std::shared_ptr<Device> mDevice;
			vk::DescriptorSetLayout mDescriptorSetLayout;
			std::map<uint32_t, vk::DescriptorSetLayoutBinding> mBindings;
		};

		class DescriptorPool;

		class DescriptorSet :public GeneralBase::NoCopyBase {
			friend class DescriptorPool;
		public:
			DescriptorSet() = default;

			DescriptorSet(DescriptorSet&& _other) noexcept;

			DescriptorSet& operator=(DescriptorSet&& _other) noexcept;

			void swap(DescriptorSet& _other) noexcept;

			~DescriptorSet();

			const vk::DescriptorSet& get() const { return mDescriptorSet; }

			void updateDescriptor(ArrayProxy<WriteDescriptorSet> _writes);

		private:
			DescriptorSet(DescriptorPool* _pool, const vk::DescriptorSet _descriptorSet)
				:mDescriptorPool(_pool), mDescriptorSet(_descriptorSet) {
			}

			DescriptorPool* mDescriptorPool = nullptr;
			vk::DescriptorSet mDescriptorSet;
		};

		class DescriptorPool :public GeneralBase::NoCopyBase {
		public:
			~DescriptorPool();

			explicit DescriptorPool(std::shared_ptr<Device> _device) :mDevice(std::move(_device)) {}

			DescriptorPool(DescriptorPool&& _other) noexcept { swap(_other); }

			DescriptorPool& operator=(DescriptorPool&& _other) noexcept { swap(_other); return *this; }

			void swap(DescriptorPool& _other) noexcept;

			void create(uint32_t _maxSets, vk::DescriptorPoolCreateFlags _flags = {});

			void addPoolSize(vk::DescriptorType _type, uint32_t _count);

			const vk::DescriptorPool& get() const { return mDescriptorPool; }

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			std::vector<DescriptorSet> allocDescriptorSet(ArrayProxy<const DescriptorSetLayout> _layouts);

			std::vector<DescriptorSet> allocDescriptorSet(const DescriptorSetLayout& _layout, const uint32_t _count);

			DescriptorSet allocDescriptorSet(const DescriptorSetLayout& _layout);

			void dealloc(DescriptorSet& _set);

			void dealloc(ArrayProxy<DescriptorSet> _sets);

			bool isIndependentFree() const { return mIndependentFree; }

		private:
			vk::DescriptorPool mDescriptorPool;
			bool mIndependentFree = false;
			std::shared_ptr<Device> mDevice;
			std::map<vk::DescriptorType, uint32_t> mPoolSizes;
		};

	}
}
#endif // !MX_VK_DESCRIPTOR_H_
