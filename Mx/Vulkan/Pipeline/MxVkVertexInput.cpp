#include "MxVkVertexInput.h"

#include <array>
#include "../MxVkUtils.h"
#include "../../Log/MxLog.h"

namespace Mix {
    namespace Vulkan {
        VertexInput::VertexInput(VertexInput&& _other) noexcept
            :mId(_other.mId),
            mBindings(std::move(_other.mBindings)),
            mAttributes(std::move(_other.mAttributes)) {
            updateVertexInputInfo();

            _other.mId = 0;
        }

        VertexInput& VertexInput::operator=(VertexInput&& _other) noexcept {
            mId = _other.mId;
            mBindings = std::move(_other.mBindings);
            mAttributes = std::move(_other.mAttributes);
            updateVertexInputInfo();

            _other.mId = 0;
            return *this;
        }

        VertexInput::VertexInput(uint32_t _id, const VertexDeclaration& _srcDecl, const VertexDeclaration& _dstDecl) :mId(_id) {
            auto& srcElems = _srcDecl.getElements();
            auto& dstElems = _dstDecl.getElements();

            uint32_t bindingCount = 0;
            uint32_t attributeCount = 0;

            for (auto& src : srcElems) {
                bool found = false;
                for (auto& dst : dstElems) {
                    if (dst.getSemantic() == src.getSemantic() &&
                        dst.getSemanticIndex() == src.getSemanticIndex() &&
                        dst.getStreamIndex() == src.getStreamIndex()) {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    continue;

                ++attributeCount;
                bindingCount = std::max(bindingCount, static_cast<uint32_t>(src.getStreamIndex() + 1));
            }

            std::vector<vk::VertexInputAttributeDescription> attributes;
            attributes.reserve(attributeCount);
            std::vector<vk::VertexInputBindingDescription> bindings;
            bindings.reserve(bindingCount);

            for (uint32_t i = 0; i < bindingCount; ++i) {
                bindings.emplace_back(i, _srcDecl.getSizeOfStream(i), vk::VertexInputRate::eVertex);
            }

            bool firstInBinding = true;
            uint32_t lastBinding = srcElems.begin()->getStreamIndex();

            for (auto& src : srcElems) {
                bool found = false;
                uint32_t location = 0;
                for (auto& dst : dstElems) {
                    if (dst.getSemantic() == src.getSemantic() &&
                        dst.getSemanticIndex() == src.getSemanticIndex() &&
                        dst.getStreamIndex() == src.getStreamIndex()) {
                        found = true;
                        location = dst.getLocation();
                        break;
                    }
                }

                if (!found)
                    continue;

                vk::VertexInputAttributeDescription attribute;
                attribute.binding = src.getStreamIndex();
                attribute.location = location;
                attribute.format = VulkanUtils::GetVertexFormat(src.getType());
                attribute.offset = src.getOffset();
                attributes.push_back(attribute);

                auto& binding = bindings[attribute.binding];
                bool perVertex = src.getInstanceRate() == 0;
                // Check input rate
                if (binding.binding != lastBinding)
                    firstInBinding = true;

                if (firstInBinding) {
                    binding.inputRate = perVertex ? vk::VertexInputRate::eVertex : vk::VertexInputRate::eInstance;
                    firstInBinding = false;
                }
                else {
                    if (binding.inputRate == vk::VertexInputRate::eVertex && !perVertex ||
                        binding.inputRate == vk::VertexInputRate::eInstance && perVertex) {
                        MX_LOG_ERROR("All attributes in one binding must have same input rate.");
                    }
                }

                lastBinding = binding.binding;
            }

            mBindings = std::move(bindings);
            mAttributes = std::move(attributes);

            updateVertexInputInfo();
        }

        void VertexInput::updateVertexInputInfo() {
            mVertexInputInfo.pVertexBindingDescriptions = mBindings.data();
            mVertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(mBindings.size());
            mVertexInputInfo.pVertexAttributeDescriptions = mAttributes.data();
            mVertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(mAttributes.size());
        }

        size_t VertexInputManager::VertexInputKey::Hash(const VertexInputKey& _v) {
            size_t hash = 0;
            Utils::HashCombine(hash, _v.srcDeclId);
            Utils::HashCombine(hash, _v.dstDeclId);
            return hash;
        }

        bool VertexInputManager::VertexInputKey::Hasher::operator()(const VertexInputKey& _key) const {
            return Hash(_key);
        }

        bool VertexInputManager::VertexInputKey::operator==(const VertexInputKey& _other) const {
            return srcDeclId == _other.srcDeclId && dstDeclId == _other.dstDeclId;
        }

        bool VertexInputManager::VertexInputKey::operator!=(const VertexInputKey& _other) const {
            return !(*this == _other);
        }

        VertexInputManager::VertexInputManager(VertexInputManager&& _other) noexcept :mNextId(_other.mNextId), mVertexInputMap(std::move(_other.mVertexInputMap)) {
        }

        VertexInputManager& VertexInputManager::operator=(VertexInputManager&& _other) noexcept {
            mNextId = _other.mNextId;
            mVertexInputMap = std::move(_other.mVertexInputMap);
            return *this;
        }

        std::shared_ptr<VertexInput> VertexInputManager::getVertexInput(const VertexDeclaration& _src, const VertexDeclaration& _dst) {
            VertexInputKey key(_src.hash(), _dst.hash());

            auto it = mVertexInputMap.find(key);
            if (it == mVertexInputMap.end()) {
                return addNew(_src, _dst);
            }
            
            return it->second;
        }

        std::shared_ptr<VertexInput> VertexInputManager::addNew(const VertexDeclaration& _src, const VertexDeclaration& _dst) {
            if (!_src.isCompatible(_dst))
                return nullptr;
            auto result = std::shared_ptr<VertexInput>(new VertexInput(++mNextId, _src, _dst));
            mVertexInputMap[VertexInputKey(_src.hash(), _dst.hash())] = result;
            return result;
        }

    }
}
