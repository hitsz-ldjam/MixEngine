#pragma once
#ifndef MX_VK_DESCRIPTOR_SET_H_
#define MX_VK_DESCRIPTOR_SET_H_

#include "../Device/MxVkDevice.h"
#include <unordered_map>
#include "../../Utils/MxArrayProxy.h"
#include <map>
#include <utility>

namespace Mix {
	namespace Graphics {
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

			void addBinding(const std::string& _name, const vk::DescriptorSetLayoutBinding& _binding);

			void create();

			const vk::DescriptorSetLayout& get() const { return mDescriptorSetLayout; }

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			std::vector<vk::DescriptorSetLayoutBinding> getBindings() const;

			std::optional<vk::DescriptorSetLayoutBinding> getBinding(const std::string& _name) const;

		private:
			std::shared_ptr<Device> mDevice;
			vk::DescriptorSetLayout mDescriptorSetLayout;
			std::unordered_map<std::string, vk::DescriptorSetLayoutBinding> mBindings;
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
