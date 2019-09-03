#pragma once
#ifndef MX_MODEL_H_
#define MX_MODEL_H_

#include <utility>
#include "../../Math/MxVector3.h"
#include "../../Math/MxQuaternion.h"
#include "../../Graphics/Mesh/MxMesh.h"

namespace Mix {
	class GameObject;
	struct GameObjectCreateInfo;

	class Model :public ResourceBase {
		friend class ModelParserBase;
	public:
		/**
		 * \brief A node in a Model.
		 */
		class Node {
		public:
			Node() = default;

			explicit Node(std::string _name) :mName(std::move(_name)) {}

			void setName(const std::string& _name) { mName = _name; }
			const std::string& getName() const { return mName; }

			void setTranslation(const Math::Vector3f& _translation) { mTranslation = _translation; }
			void setRotation(const Math::Quaternion& _rotation) { mRotation = _rotation; }
			void setScale(const Math::Vector3f& _scale) { mScale = _scale; }

			const Math::Vector3f& getTranslation() const { return mTranslation; }
			const Math::Quaternion& getRotation()const { return mRotation; }
			const Math::Vector3f& getScale()const { return mScale; }

			void setMeshRef(int32_t _meshIndex) { _meshIndex < 0 ? mMesh = -1 : mMesh = _meshIndex; }
			int32_t getMeshRef() const { return mMesh; }

			void addChildNode(const Node& _node);

			void addChildNode(Node&& _node);

			bool hasChildNode() const { return mChildren.has_value() && !mChildren.value().empty(); }

			size_t childNodeCount() const { return mChildren.has_value() ? mChildren.value().size() : 0; }

			const std::vector<Node>& getChildNodes() const { return mChildren.value(); }

			std::vector<Node>& getChildNodes() { return mChildren.value(); }

			void removeChildNode(uint32_t _index);

		private:
			std::string mName;
			// a node will have transform info
			Math::Vector3f mTranslation;
			Math::Quaternion mRotation;
			Math::Vector3f mScale = Math::Vector3f::One;

			// a node may have children
			std::optional<std::vector<Node>> mChildren;
			// a node may have a mesh
			int32_t mMesh = -1;
		};

		Model() = default;

		explicit Model(std::string _name) :mName(std::move(_name)) {}

		void setName(const std::string& _name) { mName = _name; }

		const std::string& getName() const { return mName; }

		const Node& getRootNode() const { return mRootNode; }

		Node& getRootNode() { return mRootNode; }

		void setMeshes(const std::vector<std::shared_ptr<Mesh>>& _meshes) { mMeshes = _meshes; }

		void setMeshes(std::vector<std::shared_ptr<Mesh>>&& _meshes) { mMeshes = std::move(_meshes); }

		void addMesh(std::shared_ptr<Mesh> _mesh) { mMeshes.push_back(std::move(_mesh)); }

		void addMesh(std::shared_ptr<Mesh>&& _mesh) { mMeshes.push_back(std::move(_mesh)); }

		const std::vector<std::shared_ptr<Mesh>>& getMeshes() const { return mMeshes; }

		size_t meshCount() const { return mMeshes.size(); }

		GameObject* genAllScene(const GameObjectCreateInfo& _info) const;

		GameObject* genDefaultScene(const GameObjectCreateInfo& _info) const;

	private:
		std::string mName;
		Node mRootNode;
		std::vector<std::shared_ptr<Mesh>> mMeshes;

		void recurBuildGameObj(GameObject& _obj, const Node& _node) const;
	};
}
#endif
