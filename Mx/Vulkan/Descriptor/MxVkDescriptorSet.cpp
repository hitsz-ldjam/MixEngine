#include "MxVkDescriptorSet.h"
#include "../../Log/MxLog.h"
#include "MxVkDescriptor.h"

namespace Mix {
	namespace Vulkan {

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
				mDevice->getVkHandle().destroyDescriptorSetLayout(mDescriptorSetLayout);
		}

		/*void DescriptorSetLayout::addBindings(const uint32_t _binding,
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
		}*/

		void DescriptorSetLayout::addBinding(const vk::DescriptorSetLayoutBinding& _binding) {
			mBindings[_binding.binding] = _binding;
		}

		void DescriptorSetLayout::setBindings(std::initializer_list<vk::DescriptorSetLayoutBinding> _bindings) {
			std::vector<std::pair<const uint32_t, vk::DescriptorSetLayoutBinding>> data;
			data.reserve(_bindings.size());
			for (auto& binding : _bindings) {
				data.emplace_back(binding.binding, binding);
			}

			mBindings.clear();
			mBindings.insert(data.begin(), data.end());
		}

		void DescriptorSetLayout::create() {
			if (mDevice) {
				std::vector<vk::DescriptorSetLayoutBinding> bindings;
				bindings.reserve(mBindings.size());
				for (auto& pair : mBindings)
					bindings.emplace_back(pair.second);

				vk::DescriptorSetLayoutCreateInfo createInfo;
				createInfo.pBindings = bindings.data();
				createInfo.bindingCount = static_cast<uint32_t>(bindings.size());

				mDescriptorSetLayout = mDevice->getVkHandle().createDescriptorSetLayout(createInfo);
			}
		}

		std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayout::getBindings() const {
			std::vector<vk::DescriptorSetLayoutBinding> bindings;
			bindings.reserve(mBindings.size());
			for (auto& pair : mBindings)
				bindings.emplace_back(pair.second);

			return bindings;
		}

		std::optional<vk::DescriptorSetLayoutBinding> DescriptorSetLayout::getBinding(uint32_t _binding) const {
			if (mBindings.count(_binding))
				return mBindings.at(_binding);
			return std::nullopt;
		}

		bool DescriptorSetLayout::hasBinding(uint32_t _binding) const {
			return mBindings.count(_binding);
		}

		void DescriptorPool::addPoolSize(vk::DescriptorType _type, uint32_t _count) {
			if (mPoolSizes.count(_type) == 0)
				mPoolSizes[_type] = _count;
			else
				mPoolSizes[_type] += _count;
		}

		std::vector<DescriptorSet> DescriptorPool::allocDescriptorSet(ArrayProxy<const DescriptorSetLayout> _layouts) {
			std::vector<vk::DescriptorSetLayout> layouts(_layouts.size());
			std::transform(_layouts.begin(), _layouts.end(), layouts.begin(), [](const DescriptorSetLayout& _layout) {return _layout.get(); });

			vk::DescriptorSetAllocateInfo allocInfo;
			allocInfo.descriptorPool = mDescriptorPool;
			allocInfo.pSetLayouts = layouts.data();
			allocInfo.descriptorSetCount = static_cast<uint32_t>(_layouts.size());

			auto descriptorSets = mDevice->getVkHandle().allocateDescriptorSets(allocInfo);
			auto result = std::vector<DescriptorSet>();
			result.reserve(descriptorSets.size());
			for (uint32_t i = 0; i < descriptorSets.size(); ++i)
				result.push_back(std::move(DescriptorSet(this, descriptorSets[i])));

			return result;
		}


		std::vector<DescriptorSet> DescriptorPool::allocDescriptorSet(const DescriptorSetLayout& _layout, const uint32_t _count) {
			std::vector<vk::DescriptorSetLayout> layouts(_count, _layout.get());
			vk::DescriptorSetAllocateInfo allocInfo;
			allocInfo.descriptorPool = mDescriptorPool;
			allocInfo.pSetLayouts = layouts.data();
			allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());

			auto descriptorSets = mDevice->getVkHandle().allocateDescriptorSets(allocInfo);
			auto result = std::vector<DescriptorSet>();
			result.reserve(descriptorSets.size());
			for (uint32_t i = 0; i < descriptorSets.size(); ++i)
				result.push_back(DescriptorSet(this, descriptorSets[i]));

			return result;
		}

		DescriptorSet DescriptorPool::allocDescriptorSet(const DescriptorSetLayout& _layout) {
			vk::DescriptorSetAllocateInfo allocInfo;
			allocInfo.descriptorPool = mDescriptorPool;
			allocInfo.pSetLayouts = &_layout.get();
			allocInfo.descriptorSetCount = 1;

			auto descriptorSets = mDevice->getVkHandle().allocateDescriptorSets(allocInfo);
			return DescriptorSet(this, descriptorSets.front());
		}

		DescriptorPool::~DescriptorPool() {
			if (mDescriptorPool) {
				mDevice->getVkHandle().destroyDescriptorPool(mDescriptorPool);
			}
		}

		void DescriptorPool::swap(DescriptorPool& _other) noexcept {
			using std::swap;
			swap(mDescriptorPool, _other.mDescriptorPool);
			swap(mDevice, _other.mDevice);
			swap(mPoolSizes, _other.mPoolSizes);
		}

		void DescriptorPool::create(const uint32_t _maxSets, vk::DescriptorPoolCreateFlags _flags) {
			std::vector<vk::DescriptorPoolSize> poolSizes;
			poolSizes.reserve(mPoolSizes.size());

			for (const auto& size : mPoolSizes) {
				poolSizes.emplace_back(size.first, size.second);
			}

			vk::DescriptorPoolCreateInfo createInfo;
			createInfo.pPoolSizes = poolSizes.data();
			createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			createInfo.maxSets = _maxSets;
			createInfo.flags = _flags;

			mDescriptorPool = mDevice->getVkHandle().createDescriptorPool(createInfo);
			if (_flags & vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
				mIndependentFree = true;
		}

		/*std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const std::vector<vk::DescriptorSetLayout>& _layouts) const {
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
		}*/

		void DescriptorPool::dealloc(DescriptorSet& _set) {
			mDevice->getVkHandle().freeDescriptorSets(mDescriptorPool, _set.get());
			_set.mDescriptorPool = nullptr;
			_set.mDescriptorSet = nullptr;
		}

		void DescriptorPool::dealloc(ArrayProxy<DescriptorSet> _sets) {
			auto sets = std::vector<vk::DescriptorSet>(_sets.size());
			std::transform(_sets.begin(), _sets.end(), sets.begin(), [](const DescriptorSet& _set) {return _set.get(); });
			mDevice->getVkHandle().freeDescriptorSets(mDescriptorPool, sets);
			std::for_each(_sets.begin(), _sets.end(), [](DescriptorSet& _set) {_set = DescriptorSet(); });
		}

		DescriptorSet::DescriptorSet(DescriptorSet&& _other) noexcept {
			swap(_other);
		}

		DescriptorSet& DescriptorSet::operator=(DescriptorSet&& _other) noexcept {
			swap(_other);
			return *this;
		}

		void DescriptorSet::swap(DescriptorSet& _other) noexcept {
			std::swap(mDescriptorPool, _other.mDescriptorPool);
			std::swap(mDescriptorSet, _other.mDescriptorSet);
		}

		DescriptorSet::~DescriptorSet() {
			if (mDescriptorPool && mDescriptorPool->isIndependentFree()) {
				try {
					mDescriptorPool->dealloc(*this);
				}
				catch (vk::SystemError& e) {
					std::cerr << e.what() << std::endl;
				}
			}
		}

		void DescriptorSet::updateDescriptor(ArrayProxy<WriteDescriptorSet> _writes) {
			std::vector<vk::WriteDescriptorSet> writeDescriptorSets(_writes.size());
			std::transform(_writes.begin(), _writes.end(), writeDescriptorSets.begin(), [&](WriteDescriptorSet& _w) {_w.setDstSet(this->get()); return _w.get(); });
			mDescriptorPool->getDevice()->getVkHandle().updateDescriptorSets(writeDescriptorSets, nullptr);
		}

		/*DescriptorSet::DescriptorSet(std::shared_ptr<DescriptorSetLayout> _layout, std::shared_ptr<DescriptorPool> _pool) :mDescriptorSetLayout(std::move(_layout)), mDescriptorPool(std::move(_pool)) {
			mDescriptorSet = mDescriptorPool->allocDescriptorSet(*mDescriptorSetLayout);
		}

		std::vector<vk::DescriptorSet> DescriptorPool::allocDescriptorSet(const DescriptorSetLayout & _layout, const uint32_t _count) const {
			return allocDescriptorSet(_layout.get(), _count);
		}*/
	}
}
