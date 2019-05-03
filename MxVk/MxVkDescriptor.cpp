#include "MxVkDescriptor.h"

namespace Mix {
    namespace Graphics {
        void DescriptorSetLayout::addBindings(uint32_t binding, vk::DescriptorType type, uint32_t count, vk::ShaderStageFlags stage, const vk::Sampler * immutableSamplers) {
            vk::DescriptorSetLayoutBinding layoutBinding = {};
            layoutBinding.binding = binding;
            layoutBinding.descriptorType = type;
            layoutBinding.descriptorCount = count;
            layoutBinding.stageFlags = stage;
            layoutBinding.pImmutableSamplers = immutableSamplers;
            mBindings.push_back(std::move(layoutBinding));
        }

        void DescriptorSetLayout::create() {
            vk::DescriptorSetLayoutCreateInfo createInfo = {};
            createInfo.pBindings = mBindings.data();
            createInfo.bindingCount = static_cast<uint32_t>(mBindings.size());

            mLayout = mCore->GetDevice().createDescriptorSetLayout(createInfo);
        }

        void DescriptorSetLayout::destroy() {
            mCore->GetDevice().destroyDescriptorSetLayout(mLayout);
            clear();
            mCore = nullptr;
        }

        void DescriptorPool::addPoolSize(vk::DescriptorType type, uint32_t count) {
            if (mPoolSizes.count(type) == 0)
                mPoolSizes[type] = count;
            else
                mPoolSizes[type] += count;
        }

        void DescriptorPool::create(uint32_t maxSets) {
            std::vector<vk::DescriptorPoolSize> poolSizes;
            poolSizes.reserve(mPoolSizes.size());

            for (const auto& size : mPoolSizes) {
                poolSizes.push_back({ size.first, size.second });
            }

            vk::DescriptorPoolCreateInfo createInfo;
            createInfo.pPoolSizes = poolSizes.data();
            createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            createInfo.maxSets = maxSets;

            mDescriptorPool = mCore->GetDevice().createDescriptorPool(createInfo);
        }

        std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const std::vector<vk::DescriptorSetLayout>& layouts) {
            vk::DescriptorSetAllocateInfo allocInfo;
            allocInfo.descriptorPool = mDescriptorPool;
            allocInfo.pSetLayouts = layouts.data();
            allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());

            return mCore->GetDevice().allocateDescriptorSets(allocInfo);
        }

        vk::DescriptorSet DescriptorPool::allocDescriptorSet(const vk::DescriptorSetLayout layout) {
            vk::DescriptorSetAllocateInfo allocInfo = {};
            allocInfo.descriptorPool = mDescriptorPool;
            allocInfo.pSetLayouts = &layout;
            allocInfo.descriptorSetCount = 1;

            return mCore->GetDevice().allocateDescriptorSets(allocInfo)[0];
        }

        std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const vk::DescriptorSetLayout layout, const uint32_t count) {
            std::vector<vk::DescriptorSetLayout> layouts(count, layout);
            return allocDescriptorSet(layouts);
        }

        std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const std::vector<DescriptorSetLayout>& layouts) {
            std::vector<vk::DescriptorSetLayout> tempLayouts;
            tempLayouts.reserve(layouts.size());
            for (const auto& layout : layouts)
                tempLayouts.push_back(layout.get());
            return allocDescriptorSet(tempLayouts);
        }

        vk::DescriptorSet DescriptorPool::allocDescriptorSet(const DescriptorSetLayout & layout) {
            return allocDescriptorSet(layout.get());
        }

        std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const DescriptorSetLayout & layout, const uint32_t count) {
            return allocDescriptorSet(layout.get(), count);
        }

        void DescriptorPool::destroy() {
            if (!mCore)
                return;

            mCore->GetDevice().destroyDescriptorPool(mDescriptorPool);
            mDescriptorPool = nullptr;
            mCore = nullptr;
        }
    }
}