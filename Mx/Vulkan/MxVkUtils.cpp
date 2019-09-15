#include "MxVkUtils.h"
#include "../Definitions/MxVersion.h"

namespace Mix {
    namespace Vulkan {
        vk::Format VulkanUtils::GetVertexFormat(VertexElementType _type) {
            switch (_type) {
            case VertexElementType::Float1: return vk::Format::eR32Sfloat;
            case VertexElementType::Float2: return vk::Format::eR32G32Sfloat;
            case VertexElementType::Float3: return vk::Format::eR32G32B32Sfloat;
            case VertexElementType::Float4: return vk::Format::eR32G32B32A32Sfloat;
            case VertexElementType::Int1: return vk::Format::eR32Sint;
            case VertexElementType::Int2: return vk::Format::eR32G32Sint;
            case VertexElementType::Int3: return vk::Format::eR32G32B32Sint;
            case VertexElementType::Int4: return vk::Format::eR32G32B32A32Sint;
            case VertexElementType::UInt1: return vk::Format::eR32Uint;
            case VertexElementType::UInt2: return vk::Format::eR32G32Uint;
            case VertexElementType::UInt3: return vk::Format::eR32G32B32Uint;
            case VertexElementType::UInt4: return vk::Format::eR32G32B32A32Uint;
            case VertexElementType::UByte4_Norm: return vk::Format::eR8G8B8A8Unorm;
            default: return vk::Format::eUndefined;
            }
        }

        vk::PrimitiveTopology VulkanUtils::GetTopology(MeshTopology _drawMode) {
            switch (_drawMode) {
            case MeshTopology::Points_List:		return vk::PrimitiveTopology::ePointList;
            case MeshTopology::Lines_List:		return vk::PrimitiveTopology::eLineList;
            case MeshTopology::Triangles_List:	return vk::PrimitiveTopology::eTriangleList;
            default: return vk::PrimitiveTopology::eTriangleList;
            }
        }

        uint32_t VulkanUtils::CvtToVkVersion(const Version& _version) {
            return VK_MAKE_VERSION(_version.getMajor(), _version.getMinor(), _version.getPatch());
        }
    }
}
