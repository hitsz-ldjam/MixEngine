#include "MxTransform.h"
#include "../../GameObject/MxGameObject.h"

#include <glm/gtx/quaternion.hpp>

using namespace Mix::Math;

namespace Mix {
	MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(Transform, Component);
	MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(Transform);

	Math::Vector3f Transform::forward() const {
		updateLocalToWorldMatrix();
		return mLocalToWorld.multiplyDirection(Math::Vector3f::Forward);
	}

	Math::Vector3f Transform::right() const {
		updateLocalToWorldMatrix();
		return mLocalToWorld.multiplyDirection(Math::Vector3f::Right);
	}

	Math::Vector3f Transform::up() const {
		updateLocalToWorldMatrix();
		return mLocalToWorld.multiplyDirection(Math::Vector3f::Up);
	}

	const Matrix4& Transform::localToWorldMatrix() const {
		updateLocalToWorldMatrix();
		return mLocalToWorld;
	}

	Math::Matrix4 Transform::worldToLocalMatrix() const {
		updateLocalToWorldMatrix();
		return mLocalToWorld.inverse();
	}

	Transform* Transform::root() const {
		auto p = mGameObject;
		while (p->parent() != nullptr) p = p->parent();
		return &p->transform();
	}

	void Transform::translate(const Math::Vector3f& _translation, const Space _relativeTo) {
		Vector3f tr;
		switch (_relativeTo) {
		case Space::SELF:
			tr = mQuat * _translation;
			if (mGameObject && mGameObject->parent())
				tr /= mGameObject->parent()->transform().getLocalScale();
			break;
		case Space::WORLD:
			if (mGameObject && mGameObject->parent()) {
				tr = (mGameObject->parent()->transform().worldToLocalMatrix().multiplyPoint(_translation));
			} else {
				tr = _translation;
			}
			break;
		}
		mPosition += tr;
		recurSetHasChanged();
	}

	void Transform::translate(const Math::Vector3f& _translation, const Transform& _relativeTo) {
		auto tr = _relativeTo.localToWorldMatrix().multiplyDirection(_translation);
		translate(tr, Space::WORLD);
	}

	void Transform::rotate(const Math::Quaternion& _qua, const Space _relativeTo) {
		switch (_relativeTo) {
		case Space::SELF:
			mQuat = mQuat * _qua;
			break;
		case Space::WORLD:
			if (mGameObject && mGameObject->parent()) {
				auto p = mGameObject->parent()->transform().getRotation();
				mQuat = p.inverse() * _qua * p * mQuat;
			} else
				mQuat = _qua * mQuat;
			break;
		}
		recurSetHasChanged();
	}

	void Transform::rotateAround(const Math::Vector3f& _point, const Math::Vector3f& _axis, const float _angle) {
		auto pos = getPosition();
		auto rot = Quaternion::AngleAxis(_angle, _axis);
		auto dir = pos - _point;
		dir = rot * dir;
		setPosition(_point + dir);
		auto oldRot = getRotation();
		setRotation(rot * oldRot);
	}

	void Transform::lookAt(const Math::Vector3f& _worldPosition, const Math::Vector3f& _worldUp) {
		// if worldPosition is equal to camera position, do nothing
		if (_worldPosition == getPosition())
			return;

		const Vector3f dir = (_worldPosition - getPosition()).normalize();
		float theta = dir.dot(forward());
		// if dir and forward is in same direction, do nothing
		if (EpsilonEqual(theta, 1.0f)) {
			return;
		}

		// if dir and forward is in opposite direction
		if (Math::EpsilonEqual(theta, -1.0f)) {
			setRotation(Quaternion::AngleAxis(Math::Radians(180.0f), Vector3f::Up) * getRotation());
			return;
		}

		// if dir and worldUp is in the same direction
		if (Math::EpsilonEqual(std::abs(dir.dot(_worldUp)), 1.0f)) {
			setRotation(Quaternion::FromToRotation(Vector3f::Forward, dir) * getRotation());
			return;
		}

		setRotation(Quaternion::LookRotation(dir, _worldUp));
	}

	void Transform::updateLocalToWorldMatrix() const {
		if (mChanged) {
			mLocalToWorld = Matrix4::TRS(mPosition, mQuat, mScale);
			if (mGameObject && mGameObject->parent())
				mLocalToWorld = mGameObject->parent()->transform().localToWorldMatrix()*mLocalToWorld;
			mChanged = false;
		}
	}

	void Transform::recurSetHasChanged() const {
		mChanged = true;
		if (mGameObject) {
			auto children = mGameObject->getAllChildren();
			for (auto child : children) {
				child->transform().recurSetHasChanged();
			}
		}
	}

	Vector3f Transform::getPosition() const {
		if (!mGameObject || !mGameObject->parent())
			return mPosition;
		return mGameObject->parent()->transform().localToWorldMatrix().multiplyPoint(mPosition);
	}

	void Transform::setPosition(const Math::Vector3f& _pos) {
		if (mGameObject->parent()) {
			mPosition = mGameObject->parent()->transform().worldToLocalMatrix().multiplyPoint(_pos);
		} else
			mPosition = _pos;
		recurSetHasChanged();
	}

	Math::Quaternion Transform::getRotation() const {
		// get rotation from LocalToWorldMatrix maybe slow
		// avoid using this if has no parent
		if (!mGameObject || !mGameObject->parent())
			return mQuat;

		return mGameObject->parent()->transform().getRotation() * mQuat;
	}

	void Transform::setRotation(const Math::Quaternion& _qua) {
		if (mGameObject->parent()) {
			mQuat = mGameObject->parent()->transform().worldToLocalMatrix().getRotation() * _qua;
		} else
			mQuat = _qua;
		recurSetHasChanged();
	}

	Vector3f Transform::getLossyScale() const {
		if (!mGameObject || !mGameObject->parent())
			return mScale;

		Matrix4 m = Matrix4::TRS(getPosition(), getRotation(), Vector3f::One).inverse()*localToWorldMatrix();
		return Vector3f(m[0][0], m[1][1], m[2][2]);
	}
}
