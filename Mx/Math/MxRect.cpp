#include "MxRect.h"
#include <basetsd.h>

namespace Mix {
    const Rect2i Rect2i::Empty = Rect2i();

    bool Rect2i::overlaps(const Rect2i& _other) const {
        const auto otherRight = _other.x + int32(_other.width);
        const auto myRight = x + int32(width);

        const auto otherBottom = _other.y + int32(_other.height);
        const auto myBottom = y + int32(height);

        return x <= _other.x && myRight >= otherRight &&
            y <= _other.y && myBottom >= otherBottom;
    }

    void Rect2i::transform(const Matrix4& _mat) {
        Vector4f verts[4];
        verts[0] = Vector4f(float(x), float(y), 0.0f, 1.0f);
        verts[1] = Vector4f(float(x) + width, float(y), 0.0f, 1.0f);
        verts[2] = Vector4f(float(x), float(y) + height, 0.0f, 1.0f);
        verts[3] = Vector4f(float(x) + width, float(y) + height, 0.0f, 1.0f);

        for (uint32 i = 0; i < 4; i++)
            verts[i] = _mat.multiply(verts[i]);

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();

        for (uint32 i = 0; i < 4; i++) {
            if (verts[i].x < minX)
                minX = verts[i].x;

            if (verts[i].y < minY)
                minY = verts[i].y;

            if (verts[i].x > maxX)
                maxX = verts[i].x;

            if (verts[i].y > maxY)
                maxY = verts[i].y;
        }

        x = int32(std::floor(minX));
        y = int32(std::floor(minY));
        width = uint32(int32(std::ceil(maxX)) - x);
        height = uint32(int32(std::ceil(maxY)) - y);
    }

    bool Rect2f::overlaps(const Rect2f& _other) const {
        const auto otherRight = _other.x + _other.width;
        const auto myRight = x + width;

        const auto otherBottom = _other.y + _other.height;
        const auto myBottom = y + height;

        return x <= _other.x && myRight >= otherRight &&
            y <= _other.y && myBottom >= otherBottom;
    }

    void Rect2f::transform(const Matrix4& _mat) {
        Vector4f verts[4];
        verts[0] = Vector4f(x, y, 0.0f, 1.0f);
        verts[1] = Vector4f(x + width, y, 0.0f, 1.0f);
        verts[2] = Vector4f(x, y + height, 0.0f, 1.0f);
        verts[3] = Vector4f(x + width, y + height, 0.0f, 1.0f);

        for (uint32 i = 0; i < 4; i++)
            verts[i] = _mat.multiply(verts[i]);

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();

        for (uint32 i = 0; i < 4; i++) {
            if (verts[i].x < minX)
                minX = verts[i].x;

            if (verts[i].y < minY)
                minY = verts[i].y;

            if (verts[i].x > maxX)
                maxX = verts[i].x;

            if (verts[i].y > maxY)
                maxY = verts[i].y;
        }

        x = minX;
        y = minY;
        width = maxX - x;
        height = maxY - y;
    }

    Vector2f Rect2f::getCenter() const {
        return Vector2f(x + width * 0.5f, y + height * 0.5f);
    }

    Vector2f Rect2f::getHalfExtent() const {
        return Vector2f(width * 0.5f, height * 0.5f);
    }
}
