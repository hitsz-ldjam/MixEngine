#pragma once
#ifndef MX_VK_VERTEX_INPUT_H_
#define MX_VK_VERTEX_INPUT_H_

#include <vector>
#include <vulkan/vulkan.hpp>
#include "../../Utils/MxGeneralBase.hpp"
#include "../../Definitions/MxCommonEnum.h"


namespace Mix {
	class VertexDeclaration;

	namespace Vulkan {
		class VertexInput : public GeneralBase::NoCopyBase {
			friend class VertexInputManager;
		public:
			VertexInput(VertexInput&& _other) noexcept;

			VertexInput& operator=(VertexInput&& _other) noexcept;

			const std::vector<vk::VertexInputBindingDescription>& getBindingDescription() const { return mBindings; }

			const std::vector<vk::VertexInputAttributeDescription>& getAttributeDescriptions() const { return mAttributes; }

			const vk::PipelineVertexInputStateCreateInfo& getVertexInputStateInfo() const { return mVertexInputInfo; };

			uint32_t getId() const { return mId; }

		private:
			explicit VertexInput(uint32_t _id, const VertexDeclaration& _srcDecl, const VertexDeclaration& _dstDecl);

			void updateVertexInputInfo();

			VertexInput(std::vector<vk::VertexInputBindingDescription> _bindings,
						std::vector<vk::VertexInputAttributeDescription> _attributes)
				:mBindings(std::move(_bindings)), mAttributes(std::move(_attributes)) {
			}

			uint32_t mId;
			std::vector<vk::VertexInputBindingDescription> mBindings;
			std::vector<vk::VertexInputAttributeDescription> mAttributes;
			vk::PipelineVertexInputStateCreateInfo mVertexInputInfo;
		};


		class VertexInputManager :GeneralBase::NoCopyBase {
			struct VertexInputKey {
				uint32_t srcDeclId;
				uint32_t dstDeclId;

				VertexInputKey(uint32_t _srcDeclKey, uint32_t _dstDeclKey) :srcDeclId(_srcDeclKey), dstDeclId(_dstDeclKey) {}

				static size_t Hash(const VertexInputKey& _v);

				struct Hasher {
					bool operator()(const VertexInputKey& _key) const;
				};

				bool operator==(const VertexInputKey& _other) const;

				bool operator!=(const VertexInputKey& _other) const;
			};

		public:
			VertexInputManager() = default;

			VertexInputManager(VertexInputManager&& _other) noexcept;

			VertexInputManager& operator=(VertexInputManager&& _other) noexcept;

			std::shared_ptr<VertexInput> getVertexInput(const VertexDeclaration& _src, const VertexDeclaration& _dst);

		private:
			std::shared_ptr<VertexInput> addNew(const VertexDeclaration& _src, const VertexDeclaration& _dst);

			uint32_t mNextId = 0;
			std::unordered_map<VertexInputKey, std::shared_ptr<VertexInput>, VertexInputKey::Hasher> mVertexInputMap;
		};
	}
}

#endif
