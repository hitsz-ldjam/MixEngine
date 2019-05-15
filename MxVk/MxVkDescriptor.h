#pragma once
#ifndef MX_VK_DESCRIPTOR_H_
#define MX_VK_DESCRIPTOR_H_


#include "MxVkCore.h"


namespace Mix {
    namespace Graphics {
        class DescriptorSetLayout :public GraphicsComponent {
        public:
            ~DescriptorSetLayout() { destroy(); }

            void init(const std::shared_ptr<Core>& _core) {
                setCore(_core);
            }

            void create();

            void addBindings(const uint32_t _binding,
                             const vk::DescriptorType _type,
                             const uint32_t _count,
                             const vk::ShaderStageFlags _stage,
                             const vk::Sampler* _immutableSamplers = nullptr);

            const vk::DescriptorSetLayout& get() const { return mLayout; };

            void destroy();

        private:
            vk::DescriptorSetLayout mLayout;
            std::vector<vk::DescriptorSetLayoutBinding> mBindings;

            void clear() { mBindings.clear(); }
        };

        class DescriptorPool :public GraphicsComponent {
        public:
            virtual ~DescriptorPool() { destroy(); }

            void init(const std::shared_ptr<Core>& _core) {
                setCore(_core);
            }

            void create(uint32_t _maxSets);

            void addPoolSize(vk::DescriptorType _type, uint32_t _count);

            vk::DescriptorPool get() const { return mDescriptorPool; }

            std::vector<vk::DescriptorSet> allocDescriptorSet(const std::vector<vk::DescriptorSetLayout>& _layouts) const;

            std::vector<vk::DescriptorSet> allocDescriptorSet(const vk::DescriptorSetLayout _layout, const uint32_t _count) const;

            vk::DescriptorSet allocDescriptorSet(const vk::DescriptorSetLayout _layout) const;

            std::vector<vk::DescriptorSet> allocDescriptorSet(const std::vector<DescriptorSetLayout>& _layouts) const;

            std::vector<vk::DescriptorSet> allocDescriptorSet(const DescriptorSetLayout& _layout, const uint32_t _count) const;

            vk::DescriptorSet allocDescriptorSet(const DescriptorSetLayout& _layout) const;

            void destroy();

        private:
            vk::DescriptorPool mDescriptorPool;
            std::map<vk::DescriptorType, uint32_t> mPoolSizes;
        };
    }
}
#endif // !MX_VK_DESCRIPTOR_H_
