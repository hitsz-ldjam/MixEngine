#pragma once
#ifndef _MX_VK_DESCRIPTOR_H_
#define _MX_VK_DESCRIPTOR_H_

#include"MxVkComponent.h"

#include<map>
#include<vector>

namespace Mix
{
	class MxVkDescriptorSetLayout :public MxVkComponent
	{
	private:
		VkDescriptorSetLayout mLayout;
		std::vector<VkDescriptorSetLayoutBinding> mBindings;

		void clear() { mBindings.clear(); }
	public:
		MxVkDescriptorSetLayout();
		void addBindings(uint32_t binding, VkDescriptorType type, uint32_t count,
						 VkShaderStageFlags stage,
						 const VkSampler* immutableSamplers = nullptr);
		bool createDescriptorSetLayout();
		VkDescriptorSetLayout getDescriptorSetLayout() const { return mLayout; };
		void destroy();
		~MxVkDescriptorSetLayout() { destroy(); }
	};

	class MxVkDescriptorPool :public MxVkComponent
	{
	private:
		VkDescriptorPool mDescriptorPool;

		std::map<VkDescriptorType, uint32_t> mPoolSizes;

	public:
		MxVkDescriptorPool();

		void addPoolSize(VkDescriptorType type, uint32_t count);
		bool createDescriptorPool(uint32_t maxSets);
		VkDescriptorPool getDescriptorPool() const { return mDescriptorPool; }

		std::vector<VkDescriptorSet> allocDescriptorSet(const std::vector<VkDescriptorSetLayout>& layout);
		std::vector<VkDescriptorSet> allocDescriptorSet(const VkDescriptorSetLayout layout, const uint32_t count);
		VkDescriptorSet allocDescriptorSet(const VkDescriptorSetLayout layout);

		std::vector<VkDescriptorSet> allocDescriptorSet(const std::vector<MxVkDescriptorSetLayout>& layout);
		std::vector<VkDescriptorSet> allocDescriptorSet(const MxVkDescriptorSetLayout& layout, const uint32_t count);;
		VkDescriptorSet allocDescriptorSet(const MxVkDescriptorSetLayout& layout);
		~MxVkDescriptorPool();
		void destroy();
	};
}
#endif // !_MX_VK_DESCRIPTOR_H_
