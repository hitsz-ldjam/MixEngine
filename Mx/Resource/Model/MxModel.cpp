#include "MxModel.h"
#include "../../GameObject/MxGameObject.h"
#include "../../Component/MeshFilter/MxMeshFilter.h"

namespace Mix {
	void Model::Node::addChildNode(const Node& _node) {
		if (!mChildren.has_value())
			mChildren.emplace();
		mChildren.value().push_back(_node);
	}

	void Model::Node::addChildNode(Node&& _node) {
		if (!mChildren.has_value())
			mChildren.emplace();
		mChildren.value().push_back(std::move(_node));
	}

	void Model::Node::removeChildNode(uint32_t _index) {
		if (mChildren.has_value() && _index < mChildren.value().size()) {
			mChildren.value().erase(mChildren.value().begin() + _index);
		}
	}

	GameObject* Model::genAllScene(const GameObjectCreateInfo& _info) const {
		if (!mRootNode.hasChildNode())
			return nullptr;
		auto obj = new GameObject(_info);
		recurBuildGameObj(*obj, mRootNode);
		return obj;
	}

	GameObject* Model::genDefaultScene(const GameObjectCreateInfo& _info) const {
		if (!mRootNode.hasChildNode())
			return nullptr;
		auto obj = new GameObject(_info);
		recurBuildGameObj(*obj, mRootNode.getChildNodes()[0]);
		return obj;
	}

	void Model::recurBuildGameObj(GameObject& _obj, const Node& _node) const {
		_obj.transform().setLocalPosition(_node.getTranslation());
		_obj.transform().setLocalRotation(_node.getRotation());
		_obj.transform().setLocalScale(_node.getScale());

		// if _node represents a mesh
		if (_node.getMeshRef() > -1) {
			auto filter = _obj.addComponent<MeshFilter>();
			filter->setMesh(mMeshes[_node.getMeshRef()]);
		}

		// if node has children
		if (_node.hasChildNode()) {
			for (auto& child : _node.getChildNodes()) {
				auto p = new GameObject({ child.getName() });
				recurBuildGameObj(*p, child);
				_obj.addChild(p);
			}
		}
	}
}

