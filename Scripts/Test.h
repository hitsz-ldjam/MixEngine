#pragma once
#include "../Mx/Component/Script/MxScript.h"
#include "../Mx/GameObject/MxGameObject.h"
#include "../Mx/Math/MxVector.h"

namespace Scripts {
	using namespace Mix;
	using namespace Math;

	class TestScript :public Mix::Script {
	public:
		~TestScript() = default;

	private:
		GameObject* mModel = nullptr;

		void updateMouse();

		void updatePos();

		void init() override;

		void update() override;

		void fixedUpdate() override;

		Vector3f mTarget;

		Vector2f mMouseLook;
		Vector2f mSmoothMouse;
		float mMouseSpeed = 10.0f;

		float mMaxMoveSpeed = 10.0f;
		Vector3f mSmoothMove;

		void lookAtTarget(const Vector3f& _dir);
	};
}