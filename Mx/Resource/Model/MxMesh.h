#pragma once
#ifndef MX_MESH_H_
#define MX_MESH_H_
#include <memory>
#include <vector>
#include "../../Vulkan/Buffers/MxVkBuffer.h"
#include "../MxResourceBase.h"

namespace Mix {
	namespace Resource {
		class Mesh :public ResourceBase {
			friend class ModelParseBase;
		public:
			Mesh() :mData(std::make_shared<DataType>()) {}

			struct DataType {
				struct SubmeshInfo {
					uint32_t firstVertex = 0;
					uint32_t vertexCount = 0;
					uint32_t firstIndex = 0;
					uint32_t indexCount = 0;

					SubmeshInfo() = default;

					SubmeshInfo(const uint32_t _firstVertex,
								const uint32_t _vertexCount,
								const uint32_t _firstIndex,
								const uint32_t _indexCount) :
						firstVertex(_firstVertex),
						vertexCount(_vertexCount),
						firstIndex(_firstIndex),
						indexCount(_indexCount) {
					}
				};

				std::vector<SubmeshInfo> submeshes;

				// vulkan data
				std::shared_ptr<Graphics::Buffer> vertexBuffer;

				std::shared_ptr<Graphics::Buffer> indexBuffer;
			};

			std::shared_ptr<Graphics::Buffer> getVertexBuffer() const {
				return mData->vertexBuffer;
			}

			std::shared_ptr<Graphics::Buffer> getIndexBUffer() const {
				return mData->indexBuffer;
			}

			const std::vector<DataType::SubmeshInfo>& getSubmesh() const {
				return mData->submeshes;
			}

		private:
			std::shared_ptr<DataType> mData;
		};
	}
}

#endif
