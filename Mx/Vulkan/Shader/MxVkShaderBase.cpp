#include "../../Graphics/Mesh/MxMesh.h"
#include "MxVkShaderBase.h"
#include "../CommandBuffer/MxVkCommanddBufferHandle.h"

namespace Mix {
	void Vulkan::ShaderBase::DrawMesh(CommandBufferHandle& _cmd, const Mesh& _mesh, uint32_t _submesh) {
		_cmd.get().bindVertexBuffers(0, _mesh.mVertexBuffer->get(), { 0 });
		_cmd.get().bindIndexBuffer(_mesh.mIndexBuffer->get(),
								   0,
								   _mesh.mIndexFormat == IndexFormat::UInt16 ? vk::IndexType::eUint16 : vk::IndexType::eUint32);;

		_cmd.get().drawIndexed(_mesh.mSubMeshes[_submesh].indexCount,
							   1,
							   _mesh.mSubMeshes[_submesh].firstIndex,
							   _mesh.mSubMeshes[_submesh].baseVertex,
							   0);
	}
}
