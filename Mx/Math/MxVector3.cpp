#include "MxVector3.h"

namespace Mix {
    template <>const Vector3f Vector3f::Zero = Vector3f(0.0f, 0.0f, 0.0f);
    template <>const Vector3f Vector3f::One = Vector3f(1.0f, 1.0f, 1.0f);
    template <>const Vector3f Vector3f::Up = Vector3f(0.0f, 1.0f, 0.0f);
    template <>const Vector3f Vector3f::Down = Vector3f(0.0f, -1.0f, 0.0f);
    template <>const Vector3f Vector3f::Left = Vector3f(-1.0f, 0.0f, 0.0f);
    template <>const Vector3f Vector3f::Right = Vector3f(1.0f, 0.0f, 0.0f);
    template <>const Vector3f Vector3f::Forward = Vector3f(0.0f, 0.0f, 1.0f);
    template <>const Vector3f Vector3f::Back = Vector3f(0.0f, 0.0f, -1.0f);

    template <>const Vector3d Vector3d::Zero = Vector3d(0.0, 0.0, 0.0);
    template <>const Vector3d Vector3d::One = Vector3d(1.0, 1.0, 1.0);
    template <>const Vector3d Vector3d::Up = Vector3d(0.0, 1.0, 0.0);
    template <>const Vector3d Vector3d::Down = Vector3d(0.0, -1.0, 0.0);
    template <>const Vector3d Vector3d::Left = Vector3d(-1.0, 0.0, 0.0);
    template <>const Vector3d Vector3d::Right = Vector3d(1.0, 0.0, 0.0);
    template <>const Vector3d Vector3d::Forward = Vector3d(0.0, 0.0, 1.0);
    template <>const Vector3d Vector3d::Back = Vector3d(0.0, 0.0, -1.0);

    template <>const Vector3i Vector3i::Zero = Vector3i(0, 0, 0);
    template <>const Vector3i Vector3i::One = Vector3i(1, 1, 1);
    template <>const Vector3i Vector3i::Up = Vector3i(0, 1, 0);
    template <>const Vector3i Vector3i::Down = Vector3i(0, -1, 0);
    template <>const Vector3i Vector3i::Left = Vector3i(-1, 0, 0);
    template <>const Vector3i Vector3i::Right = Vector3i(1, 0, 0);
    template <>const Vector3i Vector3i::Forward = Vector3i(0, 0.0, 1);
    template <>const Vector3i Vector3i::Back = Vector3i(0, 0, -1);

    template <>const Vector3ui Vector3ui::Zero = Vector3i(0, 0, 0);
    template <>const Vector3ui Vector3ui::One = Vector3i(1, 1, 1);
    template <>const Vector3ui Vector3ui::Up = Vector3i(0, 1, 0);
    template <>const Vector3ui Vector3ui::Right = Vector3i(1, 0, 0);
    template <>const Vector3ui Vector3ui::Forward = Vector3ui(0, 0, 1);
}
