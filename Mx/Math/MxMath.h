#pragma once
#ifndef MX_MATH_H_
#define MX_MATH_H_

#include "MxMathImpl.h"
#include "MxVector2.h"
#include "MxVector3.h"
#include "MxVector4.h"
#include "MxQuaternion.h"
#include <cmath>

namespace Mix {
    namespace Math {
        template<typename _Ty>
        static Vector2<bool> EpsilonEqual(const Vector2<_Ty>& _a, const Vector2<_Ty>& _b, _Ty const& _epsilon = static_cast<_Ty>(Math::Constants::Epsilon)) {
            return Vector2<bool>(std::abs(_a.x - _b.x) < _epsilon, std::abs(_a.y - _b.y) < _epsilon);
        }

        template<typename _Ty>
        static Vector3<bool> EpsilonEqual(const Vector3<_Ty>& _a, const Vector3<_Ty>& _b, _Ty const& _epsilon = static_cast<_Ty>(Math::Constants::Epsilon)) {
            return Vector3<bool>(std::abs(_a.x - _b.x) < _epsilon,
                                 std::abs(_a.y - _b.y) < _epsilon,
                                 std::abs(_a.z - _b.z) < _epsilon);
        }

        template<typename _Ty>
        static Vector4<bool> EpsilonEqual(const Vector4<_Ty>& _a, const Vector4<_Ty>& _b, _Ty const& _epsilon = static_cast<_Ty>(Math::Constants::Epsilon)) {
            return Vector4<bool>(std::abs(_a.x - _b.x) < _epsilon,
                                 std::abs(_a.y - _b.y) < _epsilon,
                                 std::abs(_a.z - _b.z) < _epsilon,
                                 std::abs(_a.w - _b.w) < _epsilon);
        }

        template<>
        static Quaternion Zero<Quaternion>() {
            return Quaternion(0.0f, 0.0f, 0.0f , 0.0f);
        }
    }
}

#endif
