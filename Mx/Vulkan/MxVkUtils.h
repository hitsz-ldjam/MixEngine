#pragma once
#ifndef MX_VK_UTILS_H_
#define MX_VK_UTILS_H_
#include "../Utils/MxGeneralBase.hpp"
#include <vulkan/vulkan.hpp>
#include "../Graphics/MxVertexDeclaration.h"
#include "../Definitions/MxCommonType.h"


namespace Mix {
    class Version;

	namespace Vulkan {
		class VulkanUtils:public GeneralBase::StaticBase {
		public:
			static vk::Format GetVertexFormat(VertexElementType _type);

			static vk::PrimitiveTopology GetTopology(MeshTopology _drawMode);

            static uint32_t CvtToVkVersion(const Version& _version);
		};
	}
}

#endif
