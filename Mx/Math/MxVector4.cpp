#include "MxVector4.h"

namespace Mix {
    template <>const Vector4f Vector4f::Zero = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    template <>const Vector4f Vector4f::One = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    template <>const Vector4d Vector4d::Zero = Vector4d(0.0, 0.0, 0.0, 0.0);
    template <>const Vector4d Vector4d::One = Vector4d(1.0, 1.0, 1.0, 1.0);

    template <>const Vector4i Vector4i::Zero = Vector4i(0, 0, 0, 0);
    template <>const Vector4i Vector4i::One = Vector4i(1, 1, 1, 1);

    template <>const Vector4ui Vector4ui::Zero = Vector4i(0, 0, 0, 0);
    template <>const Vector4ui Vector4ui::One = Vector4i(1, 1, 1, 1);
}
