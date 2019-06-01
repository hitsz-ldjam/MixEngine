#pragma once
#ifndef MX_PTR_MAKE_H_
#define MX_PTR_MAKE_H_

namespace Mix {
	namespace Math {
		template<typename _Ty>
		class Vector2;

		template<typename _Ty>
		class Vector3;

		template<typename _Ty>
		class Vector4;

		class Matrix4;

		template<typename _Ty, typename _Rty = _Ty>
		Vector2<_Rty> MakeVector2(_Ty const* const _ptr) {

		}

		template<typename _Ty, typename _Rty = _Ty>
		Vector2<_Rty> MakeVector3(_Ty const* const _ptr) {

		}

		template<typename _Ty, typename _Rty = _Ty>
		Vector2<_Rty> MakeVector4(_Ty const* const _ptr) {

		}

	}
}

#endif