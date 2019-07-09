#pragma once
#ifndef MX_MODEL_H_
#define MX_MODEL_H_

#include "../../Vulkan/Memory/MxVkAllocator.h"
#include "../../Math/MxVector3.h"
#include "../../Math/MxQuaternion.h"
#include "MxMesh.h"

namespace Mix {
	class GameObject;
	struct GameObjectConstructionInfo;

	namespace Resource {
		class Model :public ResourceBase {
			friend class ModelParserBase;
		public:
			Model() :mData(std::make_shared<DataType>()) {}

			struct DataType {
				struct Node {
					std::string name;

					// a node will have transform info
					Math::Vector3f translation;
					Math::Quaternion rotation;
					Math::Vector3f scale = Math::Vector3f::One;

					std::optional<std::vector<Node>> children; // a node may have children

					int32_t mesh = -1; // a node may represent a mesh
				};

				Node rootNode;

				std::vector<std::shared_ptr<Mesh>> meshes;
			};

			GameObject* generateGameObject(const GameObjectConstructionInfo& _info) const;

		private:
			std::shared_ptr<DataType> mData;

			void recurBuildGameObj(GameObject& _obj, DataType::Node& _node) const;
		};

	}
}
#endif
