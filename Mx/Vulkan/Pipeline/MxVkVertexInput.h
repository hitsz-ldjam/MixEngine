#pragma once
#ifndef MX_VK_VERTEX_INPUT_H_
#define MX_VK_VERTEX_INPUT_H_

#include <vector>
#include <vulkan/vulkan.hpp>
#include "../../Resource/Model/MxMesh.h"

namespace Mix {
	namespace Vulkan {
		class VertexInput {
		public:
			explicit VertexInput(Flags<VertexAttribute> _srcVertexAttributeFlags, Flags<VertexAttribute> _dstVertexAttributeFlags);

			explicit VertexInput(Flags<VertexAttribute> _vertexAttributeFlags);

			VertexInput(std::vector<vk::VertexInputBindingDescription> _bindings,
						std::vector<vk::VertexInputAttributeDescription> _attributes)
				:mBindings(std::move(_bindings)), mAttributes(std::move(_attributes)) {
			}

			std::vector<vk::VertexInputBindingDescription> getBindingDescription() const { return mBindings; }

			std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() const { return mAttributes; }

			static vk::Format VertexAttributeToVkFormat(VertexAttribute _vertexAttribute);

			static uint32_t GetVertexAttributeSize(VertexAttribute _vertexAttribute);

		private:
			std::vector<vk::VertexInputBindingDescription> mBindings;
			std::vector<vk::VertexInputAttributeDescription> mAttributes;

			static std::array<VertexAttribute, 6> sAttributes;
		};
	}
}

#endif
