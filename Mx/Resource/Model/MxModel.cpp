#include "MxModel.h"
#include "../../GameObject/MxGameObject.h"
#include "../../Component/MeshFilter/MxMeshFilter.h"
#include "../../Scene/MxSceneManager.h"

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

    HGameObject Model::genAllGameObjects(const std::string& _name,
                                         const Tag& _tag,
                                         const LayerIndex _layerIndex,
                                         Flags<GameObjectFlags> _flags) const {
        return genAllGameObjects(SceneManager::Get()->getActiveScene(), _name, _tag, _layerIndex, _flags);
    }

    HGameObject Model::genAllGameObjects(const std::shared_ptr<Scene>& _scene,
                                         const std::string& _name,
                                         const Tag& _tag,
                                         LayerIndex _layerIndex,
                                         Flags<GameObjectFlags> _flags) const {
        if (!mRootNode.hasChildNode())
            return nullptr;
        auto obj = GameObject::CreateInternal(_scene, _name, _tag, _layerIndex, _flags);
        recurBuildGameObj(_scene, obj, mRootNode.getChildNodes()[0]);
        return obj;
    }

    HGameObject Model::genDefaultGameObjects(const std::string& _name,
                                             const Tag& _tag,
                                             LayerIndex _layerIndex,
                                             Flags<GameObjectFlags> _flags) const {
        return genDefaultGameObjects(SceneManager::Get()->getActiveScene(), _name, _tag, _layerIndex, _flags);
    }

    HGameObject Model::genDefaultGameObjects(const std::shared_ptr<Scene>& _scene,
                                             const std::string& _name,
                                             const Tag& _tag,
                                             LayerIndex _layerIndex,
                                             Flags<GameObjectFlags> _flags) const {
        if (!mRootNode.hasChildNode())
            return nullptr;
        auto obj = GameObject::CreateInternal(_scene, _name, _tag, _layerIndex, _flags);
        recurBuildGameObj(_scene, obj, mRootNode.getChildNodes()[0]);
        return obj;
    }

    void Model::recurBuildGameObj(const std::shared_ptr<Scene>& _scene, const HGameObject& _obj, const Node& _node) const {
        _obj->transform().setLocalPosition(_node.getTranslation());
        _obj->transform().setLocalRotation(_node.getRotation());
        _obj->transform().setLocalScale(_node.getScale());

        // if _node represents a mesh
        if (_node.getMeshRef() > -1) {
            auto filter = _obj->addComponent<MeshFilter>();
            filter->setMesh(mMeshes[_node.getMeshRef()]);
        }

        // if node has children
        if (_node.hasChildNode()) {
            for (auto& child : _node.getChildNodes()) {
                auto p = GameObject::Instantiate(child.getName(), _obj->getTag(), _obj->getLayer(), _obj->getFlags());
                recurBuildGameObj(_scene, p, child);
                p->setParent(_obj);
            }
        }
    }
}

