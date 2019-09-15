#pragma once
#ifndef MX_PTR_MAKE_H_
#define MX_PTR_MAKE_H_

namespace Mix {
		template<typename _Ty>
		class Vector2;

		template<typename _Ty>
		class Vector3;

		template<typename _Ty>
		class Vector4;

		class Matrix4;

		template<typename _Ty, typename _Rty = _Ty>
		Vector2<_Rty> MakeVector2(_Ty const* const _ptr) {
			return Vector2<_Rty>(_ptr[0], _ptr[1]);
		}

		template<typename _Ty, typename _Rty = _Ty>
		Vector3<_Rty> MakeVector3(_Ty const* const _ptr) {
			return Vector3<_Rty>(_ptr[0], _ptr[1], _ptr[2]);
		}

		template<typename _Ty, typename _Rty = _Ty>
		Vector4<_Rty> MakeVector4(_Ty const* const _ptr) {
			return Vector4<_Rty>(_ptr[0], _ptr[1], _ptr[2], _ptr[3]);

		}

		template<typename _Ty>
		Quaternion MakeQuaternion(_Ty const* const _ptr) {
			return Quaternion(static_cast<float>(_ptr[0]),
							  static_cast<float>(_ptr[1]),
							  static_cast<float>(_ptr[2]),
							  static_cast<float>(_ptr[3]));
		}

		template<typename _Ty>
		Matrix4 MakeMatrix4(_Ty const* const _ptr) {
			return Matrix4(static_cast<float>(_ptr[0]),
						   static_cast<float>(_ptr[1]),
						   static_cast<float>(_ptr[2]),
						   static_cast<float>(_ptr[3]),
						   static_cast<float>(_ptr[4]),
						   static_cast<float>(_ptr[5]),
						   static_cast<float>(_ptr[6]),
						   static_cast<float>(_ptr[7]),
						   static_cast<float>(_ptr[8]),
						   static_cast<float>(_ptr[9]),
						   static_cast<float>(_ptr[10]),
						   static_cast<float>(_ptr[11]),
						   static_cast<float>(_ptr[12]),
						   static_cast<float>(_ptr[13]),
						   static_cast<float>(_ptr[14]),
						   static_cast<float>(_ptr[15]));
		}
}

#endif