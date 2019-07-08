#include "MxVkDescriptor.h"

namespace Mix {
	namespace Graphics {

		DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayout& _other)
			:mDevice(_other.mDevice), mBindings(_other.mBindings) {
			create();
		}

		DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& _other) noexcept {
			swap(_other);
		}

		DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout _other) {
			swap(_other);
			return *this;
		}

		DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& _other) noexcept {
			swap(_other);
			return *this;
		}

		void DescriptorSetLayout::swap(DescriptorSetLayout& _other) noexcept {
			std::swap(mDevice, _other.mDevice);
			std::swap(mDescriptorSetLayout, _other.mDescriptorSetLayout);
			std::swap(mBindings, _other.mBindings);
		}

		DescriptorSetLayout::~DescriptorSetLayout() {
			if (mDescriptorSetLayout)
				mDevice->get().destroyDescriptorSetLayout(mDescriptorSetLayout);
		}

		void DescriptorSetLayout::addBindings(const uint32_t _binding,
											  const vk::DescriptorType _type,
											  const uint32_t _count,
											  const vk::ShaderStageFlags _stage,
											  const vk::Sampler * _immutableSamplers) {
			vk::DescriptorSetLayoutBinding layoutBinding;
			layoutBinding.binding = _binding;
			layoutBinding.descriptorType = _type;
			layoutBinding.descriptorCount = _count;
			layoutBinding.stageFlags = _stage;
			layoutBinding.pImmutableSamplers = _immutableSamplers;
			mBindings.push_back(std::move(layoutBinding));
		}

		void DescriptorSetLayout::addBindings(ArrayProxy<const vk::DescriptorSetLayoutBinding> _bindings) {
			mBindings.insert(mBindings.end(), _bindings.begin(), _bindings.end());
		}

		void DescriptorSetLayout::create() {
			if (mDevice) {
				vk::DescriptorSetLayoutCreateInfo createInfo = {};
				createInfo.pBindings = mBindings.data();
				createInfo.bindingCount = static_cast<uint32_t>(mBindings.size());

				mDescriptorSetLayout = mDevice->get().createDescriptorSetLayout(createInfo);
			}
		}

		void DescriptorPool::addPoolSize(vk::DescriptorType _type, uint32_t _count) {
			if (mPoolSizes.count(_type) == 0)
				mPoolSizes[_type] = _count;
			else
				mPoolSizes[_type] += _count;
		}

		DescriptorPool::~DescriptorPool() {
			if (mDescriptorPool) {
				mDevice->get().destroyDescriptorPool(mDescriptorPool);
			}
		}

		void DescriptorPool::swap(DescriptorPool& _other) noexcept {
			using std::swap;
			swap(mDescriptorPool, _other.mDescriptorPool);
			swap(mDevice, _other.mDevice);
			swap(mPoolSizes, _other.mPoolSizes);
		}

		void DescriptorPool::create(const uint32_t _maxSets) {
			std::vector<vk::DescriptorPoolSize> poolSizes;
			poolSizes.reserve(mPoolSizes.size());

			for (const auto& size : mPoolSizes) {
				poolSizes.emplace_back(size.first, size.second);
			}

			vk::DescriptorPoolCreateInfo createInfo;
			createInfo.pPoolSizes = poolSizes.data();
			createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			createInfo.maxSets = _maxSets;

			mDescriptorPool = mDevice->get().createDescriptorPool(createInfo);
		}

		std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const std::vector<vk::DescriptorSetLayout>& _layouts) const {
			vk::DescriptorSetAllocateInfo allocInfo;
			allocInfo.descriptorPool = mDescriptorPool;
			allocInfo.pSetLayouts = _layouts.data();
			allocInfo.descriptorSetCount = static_cast<uint32_t>(_layouts.size());

			return mDevice->get().allocateDescriptorSets(allocInfo);
		}

		vk::DescriptorSet DescriptorPool::allocDescriptorSet(const vk::DescriptorSetLayout _layout) const {
			vk::DescriptorSetAllocateInfo allocInfo = {};
			allocInfo.descriptorPool = mDescriptorPool;
			allocInfo.pSetLayouts = &_layout;
			allocInfo.descriptorSetCount = 1;

			return mDevice->get().allocateDescriptorSets(allocInfo)[0];
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

		void DescriptorPool::dealloc(const vk::DescriptorSet& _set) const {
			mDevice->get().freeDescriptorSets(mDescriptorPool, _set);
		}

		void DescriptorPool::dealloc(const std::vector<vk::DescriptorSet>& _sets) const {
			mDevice->get().freeDescriptorSets(mDescriptorPool, _sets);
		}

		std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const DescriptorSetLayout & _layout, const uint32_t _count) const {
			return allocDescriptorSet(_layout.get(), _count);
		}
	}
}