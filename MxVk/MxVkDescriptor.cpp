#include "MxVkDescriptor.h"

namespace Mix {
    namespace Graphics {
        void DescriptorSetLayout::addBindings(const uint32_t _binding,
                                              const vk::DescriptorType _type,
                                              const uint32_t _count,
                                              const vk::ShaderStageFlags _stage, 
                                              const vk::Sampler * _immutableSamplers) {
            vk::DescriptorSetLayoutBinding layoutBinding = {};
            layoutBinding.binding = _binding;
            layoutBinding.descriptorType = _type;
            layoutBinding.descriptorCount = _count;
            layoutBinding.stageFlags = _stage;
            layoutBinding.pImmutableSamplers = _immutableSamplers;
            mBindings.push_back(std::move(layoutBinding));
        }

        void DescriptorSetLayout::create() {
            vk::DescriptorSetLayoutCreateInfo createInfo = {};
            createInfo.pBindings = mBindings.data();
            createInfo.bindingCount = static_cast<uint32_t>(mBindings.size());

            mLayout = mCore->getDevice().createDescriptorSetLayout(createInfo);
        }

        void DescriptorSetLayout::destroy() {
            mCore->getDevice().destroyDescriptorSetLayout(mLayout);
            clear();
            mCore = nullptr;
        }

        void DescriptorPool::addPoolSize(vk::DescriptorType _type, uint32_t _count) {
            if (mPoolSizes.count(_type) == 0)
                mPoolSizes[_type] = _count;
            else
                mPoolSizes[_type] += _count;
        }

        void DescriptorPool::create(uint32_t _maxSets) {
            std::vector<vk::DescriptorPoolSize> poolSizes;
            poolSizes.reserve(mPoolSizes.size());

            for (const auto& size : mPoolSizes) {
                poolSizes.push_back({ size.first, size.second });
            }

            vk::DescriptorPoolCreateInfo createInfo;
            createInfo.pPoolSizes = poolSizes.data();
            createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            createInfo.maxSets = _maxSets;

            mDescriptorPool = mCore->getDevice().createDescriptorPool(createInfo);
        }

        std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const std::vector<vk::DescriptorSetLayout>& _layouts) const {
            vk::DescriptorSetAllocateInfo allocInfo;
            allocInfo.descriptorPool = mDescriptorPool;
            allocInfo.pSetLayouts = _layouts.data();
            allocInfo.descriptorSetCount = static_cast<uint32_t>(_layouts.size());

            return mCore->getDevice().allocateDescriptorSets(allocInfo);
        }

        vk::DescriptorSet DescriptorPool::allocDescriptorSet(const vk::DescriptorSetLayout _layout) const {
            vk::DescriptorSetAllocateInfo allocInfo = {};
            allocInfo.descriptorPool = mDescriptorPool;
            allocInfo.pSetLayouts = &_layout;
            allocInfo.descriptorSetCount = 1;

            return mCore->getDevice().allocateDescriptorSets(allocInfo)[0];
        }

        std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const vk::DescriptorSetLayout _layout, const uint32_t _count) const {
            std::vector<vk::DescriptorSetLayout> layouts(_count, _layout);
            return allocDescriptorSet(layouts);
        }

        std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const std::vector<DescriptorSetLayout>& _layouts) const {
            std::vector<vk::DescriptorSetLayout> tempLayouts;
            tempLayouts.reserve(_layouts.size());
            for (const auto& layout : _layouts)
                tempLayouts.push_back(layout.get());
            return allocDescriptorSet(tempLayouts);
        }

        vk::DescriptorSet DescriptorPool::allocDescriptorSet(const DescriptorSetLayout & _layout) const {
            return allocDescriptorSet(_layout.get());
        }

        std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const DescriptorSetLayout & _layout, const uint32_t _count) const {
            return allocDescriptorSet(_layout.get(), _count);
        }

        void DescriptorPool::destroy() {
            if (!mCore)
                return;

            mCore->getDevice().destroyDescriptorPool(mDescriptorPool);
            mDescriptorPool = nullptr;
            mCore = nullptr;
        }
    }
}