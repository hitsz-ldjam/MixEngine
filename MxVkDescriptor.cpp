#include "MxVkDescriptor.h"

namespace Mix
{
	MxVkDescriptorPool::MxVkDescriptorPool() :mDescriptorPool(VK_NULL_HANDLE)
	{
	}

	void MxVkDescriptorPool::addPoolSize(VkDescriptorType type, uint32_t count)
	{
		if (mPoolSizes.count(type) == 0)
			mPoolSizes[type] = 1;
		else
			mPoolSizes[type] += count;
	}

	bool MxVkDescriptorPool::createDescriptorPool(uint32_t maxSets)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;
		poolSizes.reserve(mPoolSizes.size());

		for (const auto& size : mPoolSizes)
		{
			poolSizes.push_back({ size.first, size.second });
		}

		VkDescriptorPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.pPoolSizes = poolSizes.data();
		createInfo.poolSizeCount = poolSizes.size();
		createInfo.maxSets = maxSets;

		MX_VK_CHECK_RESULT(vkCreateDescriptorPool(mManager->getDevice(), &createInfo, nullptr, &mDescriptorPool));
		mPoolSizes.clear();
		return true;
	}

	std::vector<VkDescriptorSet> MxVkDescriptorPool::allocDescriptorSet(const std::vector<VkDescriptorSetLayout>& layouts)
	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = mDescriptorPool;
		allocInfo.pSetLayouts = layouts.data();
		allocInfo.descriptorSetCount = layouts.size();

		std::vector<VkDescriptorSet> tempSets(layouts.size());
		MX_VK_CHECK_RESULT(vkAllocateDescriptorSets(mManager->getDevice(), &allocInfo, tempSets.data()));
		return tempSets;
	}

	VkDescriptorSet MxVkDescriptorPool::allocDescriptorSet(const VkDescriptorSetLayout layout)
	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = mDescriptorPool;
		allocInfo.pSetLayouts = &layout;
		allocInfo.descriptorSetCount = 1;

		VkDescriptorSet tempSet;
		MX_VK_CHECK_RESULT(vkAllocateDescriptorSets(mManager->getDevice(), &allocInfo, &tempSet));
		return tempSet;
	}

	std::vector<VkDescriptorSet> MxVkDescriptorPool::allocDescriptorSet(const VkDescriptorSetLayout layout, const uint32_t count)
	{
		std::vector<VkDescriptorSetLayout> layouts(count, layout);
		return allocDescriptorSet(layouts);
	}

	std::vector<VkDescriptorSet> MxVkDescriptorPool::allocDescriptorSet(const std::vector<MxVkDescriptorSetLayout>& layouts)
	{
		std::vector<VkDescriptorSetLayout> tempLayouts;
		tempLayouts.reserve(layouts.size());
		for (const auto& layout : layouts)
			tempLayouts.push_back(layout.getDescriptorSetLayout());
		return allocDescriptorSet(tempLayouts);
	}

	VkDescriptorSet MxVkDescriptorPool::allocDescriptorSet(const MxVkDescriptorSetLayout & layout)
	{
		return allocDescriptorSet(layout.getDescriptorSetLayout());
	}

	std::vector<VkDescriptorSet> MxVkDescriptorPool::allocDescriptorSet(const MxVkDescriptorSetLayout & layout, const uint32_t count)
	{
		return allocDescriptorSet(layout.getDescriptorSetLayout(), count);
	}

	MxVkDescriptorPool::~MxVkDescriptorPool()
	{
		destroy();
	}

	void MxVkDescriptorPool::destroy()
	{
		if (!mIsReady)
			return;
		vkDestroyDescriptorPool(mManager->getDevice(), mDescriptorPool, nullptr);
		mManager = nullptr;
		mIsReady = false;
	}

	MxVkDescriptorSetLayout::MxVkDescriptorSetLayout() :mLayout(VK_NULL_HANDLE)
	{
	}


	void MxVkDescriptorSetLayout::addBindings(uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stage, const VkSampler * immutableSamplers)
	{
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = type;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stage;
		layoutBinding.pImmutableSamplers = immutableSamplers;
		mBindings.push_back(std::move(layoutBinding));
	}

	bool MxVkDescriptorSetLayout::createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.pBindings = mBindings.data();
		layoutInfo.bindingCount = mBindings.size();

		VkDescriptorSetLayout layout;
		MX_VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mManager->getDevice(), &layoutInfo, nullptr, &layout));
		mLayout = layout;
		return true;
	}

	void MxVkDescriptorSetLayout::destroy()
	{
		if (!mIsReady)
			return;

		if (mLayout != VK_NULL_HANDLE)
			vkDestroyDescriptorSetLayout(mManager->getDevice(), mLayout, nullptr);
		clear();
		mManager = nullptr;
		mIsReady = false;
	}

}