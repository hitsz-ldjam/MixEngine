#include "MxVkVertexInput.h"

#include <array>

namespace Mix {
	namespace Vulkan {
		std::array<VertexAttribute, 6> VertexInput::sAttributes = {
			VertexAttribute::POSITION,
			VertexAttribute::NORMAL,
			VertexAttribute::TANGENT,
			VertexAttribute::UV0,
			VertexAttribute::UV1,
			VertexAttribute::COLOR
		};

		VertexInput::VertexInput(Flags<VertexAttribute> _srcVertexAttributeFlags, Flags<VertexAttribute> _dstVertexAttributeFlags) {
			uint32_t stride = 0;
			uint32_t location = 0;
			std::vector<vk::VertexInputAttributeDescription> attributeDescrs;
			for (auto attribute : sAttributes) {
				if (_srcVertexAttributeFlags.isSet(attribute)) {
					if (_dstVertexAttributeFlags.isSet(attribute)) {
						vk::VertexInputAttributeDescription attributeDescr;
						attributeDescr.binding = 0;
						attributeDescr.location = location++;
						attributeDescr.format = VertexAttributeToVkFormat(attribute);
						attributeDescr.offset = stride;
						attributeDescrs.push_back(attributeDescr);
					}
					stride += GetVertexAttributeSize(attribute);
				}
			}

			vk::VertexInputBindingDescription bindingDesc;
			bindingDesc.binding = 0;
			bindingDesc.inputRate = vk::VertexInputRate::eVertex;
			bindingDesc.stride = stride;

			mBindings.push_back(bindingDesc);
			mAttributes = std::move(attributeDescrs);
		}

		VertexInput::VertexInput(Flags<VertexAttribute> _vertexAttributeFlags) {
			uint32_t stride = 0;
			uint32_t location = 0;
			std::vector<vk::VertexInputAttributeDescription> attributeDescrs;

			for (auto attribute : sAttributes) {
				if (_vertexAttributeFlags.isSet(attribute)) {
					vk::VertexInputAttributeDescription attributeDescr;
					attributeDescr.binding = 0;
					attributeDescr.location = location++;
					attributeDescr.format = VertexAttributeToVkFormat(attribute);
					attributeDescr.offset = stride;
					attributeDescrs.push_back(attributeDescr);
					stride += GetVertexAttributeSize(attribute);
				}
			}

			vk::VertexInputBindingDescription bindingDesc;
			bindingDesc.binding = 0;
			bindingDesc.inputRate = vk::VertexInputRate::eVertex;
			bindingDesc.stride = stride;

			mBindings.push_back(bindingDesc);
			mAttributes = std::move(attributeDescrs);
		}

		vk::Format VertexInput::VertexAttributeToVkFormat(VertexAttribute _vertexAttribute) {
			switch (_vertexAttribute) {
			case VertexAttribute::POSITION: return vk::Format::eR32G32B32Sfloat;
			case VertexAttribute::NORMAL:	return vk::Format::eR32G32B32Sfloat;
			case VertexAttribute::TANGENT:	return vk::Format::eR32G32B32Sfloat;
			case VertexAttribute::UV0:		return vk::Format::eR32G32Sfloat;
			case VertexAttribute::UV1:		return vk::Format::eR32G32Sfloat;
			case VertexAttribute::COLOR:	return vk::Format::eR8G8B8A8Unorm;
			default: return vk::Format::eUndefined;
			}
		}

		uint32_t VertexInput::GetVertexAttributeSize(VertexAttribute _vertexAttribute) {
			switch (_vertexAttribute) {
			case VertexAttribute::POSITION: return 12;
			case VertexAttribute::NORMAL:	return 12;
			case VertexAttribute::TANGENT:	return 12;
			case VertexAttribute::UV0:		return 8;
			case VertexAttribute::UV1:		return 8;
			case VertexAttribute::COLOR:	return 4;
			default: return 0;
			}
		}
	}
}