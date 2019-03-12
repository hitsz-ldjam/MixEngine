#include "MxVkDescriptor.h"

namespace Mix {
    namespace Graphics {
        MX_IMPLEMENT_RTTI_NoCreateFunc(DescriptorSetLayout, GraphicsComponent);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(DescriptorSetLayout);

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
            createInfo.bindingCount = mBindings.size();

            mLayout = mCore->device().createDescriptorSetLayout(createInfo);
        }

        void DescriptorSetLayout::destroy() {
            mCore->device().destroyDescriptorSetLayout(mLayout);
            clear();
            mCore = nullptr;
        }

        MX_IMPLEMENT_RTTI_NoCreateFunc(DescriptorPool, GraphicsComponent);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(DescriptorPool);

        void DescriptorPool::addPoolSize(vk::DescriptorType type, uint32_t count) {
            if (mPoolSizes.count(type) == 0)
                mPoolSizes[type] = 1;
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
            createInfo.poolSizeCount = poolSizes.size();
            createInfo.maxSets = maxSets;

            mDescriptorPool = mCore->device().createDescriptorPool(createInfo);
            mPoolSizes.clear();
        }

        std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const std::vector<vk::DescriptorSetLayout>& layouts) {
            vk::DescriptorSetAllocateInfo allocInfo;
            allocInfo.descriptorPool = mDescriptorPool;
            allocInfo.pSetLayouts = layouts.data();
            allocInfo.descriptorSetCount = layouts.size();

            return mCore->device().allocateDescriptorSets(allocInfo);
        }

        vk::DescriptorSet DescriptorPool::allocDescriptorSet(const vk::DescriptorSetLayout layout) {
            vk::DescriptorSetAllocateInfo allocInfo = {};
            allocInfo.descriptorPool = mDescriptorPool;
            allocInfo.pSetLayouts = &layout;
            allocInfo.descriptorSetCount = 1;

            return mCore->device().allocateDescriptorSets(allocInfo)[0];
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
            mCore->device().destroyDescriptorPool(mDescriptorPool);
            mDescriptorPool = VK_NULL_HANDLE;
            mCore = nullptr;
        }
    }
}