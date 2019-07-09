#include "MxModel.h"
#include "../../GameObject/MxGameObject.h"
#include "../../Component/MeshFilter/MxMeshFilter.h"

namespace Mix {
	namespace Resource {
		GameObject* Model::generateGameObject(const GameObjectConstructionInfo& _info) const {
			if (!mData)
				return nullptr;
            auto obj = new GameObject(_info);
			recurBuildGameObj(*obj, mData->rootNode);
			return obj;
		}

		void Model::recurBuildGameObj(GameObject& _obj, DataType::Node& _node) const {
			_obj.transform().setLocalPosition(_node.translation);
			_obj.transform().setLocalRotation(_node.rotation);
			_obj.transform().setLocalScale(_node.scale);

			// if _node represents a mesh
			if (_node.mesh > -1) {
				auto filter = _obj.addComponent<MeshFilter>();
				filter->setMesh(mData->meshes[_node.mesh]);
			}

			// if node has children
			if(_node.children) {
				for(auto& child:_node.children.value()) {
                    auto p = new GameObject({child.name});
					recurBuildGameObj(*p, child);
					_obj.addChild(p);
				}
			}
		}
	}
}
