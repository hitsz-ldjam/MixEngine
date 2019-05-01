#pragma once
#ifndef _MX_VK_DESCRIPTOR_H_
#define _MX_VK_DESCRIPTOR_H_


#include "MxVkCore.h"


namespace Mix {
    namespace Graphics {
        class DescriptorSetLayout :public GraphicsComponent {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            virtual ~DescriptorSetLayout() { destroy(); }

            void create();

            void addBindings(uint32_t binding,
                             vk::DescriptorType type,
                             uint32_t count,
                             vk::ShaderStageFlags stage,
                             const vk::Sampler* immutableSamplers = nullptr);

            vk::DescriptorSetLayout get() const { return mLayout; };

            void destroy();

        private:
            vk::DescriptorSetLayout mLayout;
            std::vector<vk::DescriptorSetLayoutBinding> mBindings;

            void clear() { mBindings.clear(); }
        };

        class DescriptorPool :public GraphicsComponent {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            virtual ~DescriptorPool() { destroy(); }

            void create(uint32_t maxSets);

            void addPoolSize(vk::DescriptorType type, uint32_t count);

            vk::DescriptorPool get() const { return mDescriptorPool; }

            std::vector<vk::DescriptorSet> allocDescriptorSet(const std::vector<vk::DescriptorSetLayout>& layout);
            std::vector<vk::DescriptorSet> allocDescriptorSet(const vk::DescriptorSetLayout layout, const uint32_t count);
            vk::DescriptorSet allocDescriptorSet(const vk::DescriptorSetLayout layout);

            std::vector<vk::DescriptorSet> allocDescriptorSet(const std::vector<DescriptorSetLayout>& layout);
            std::vector<vk::DescriptorSet> allocDescriptorSet(const DescriptorSetLayout& layout, const uint32_t count);;
            vk::DescriptorSet allocDescriptorSet(const DescriptorSetLayout& layout);

            void destroy();

        private:
            vk::DescriptorPool mDescriptorPool;
            std::map<vk::DescriptorType, uint32_t> mPoolSizes;
        };
    }
}
#endif // !_MX_VK_DESCRIPTOR_H_
