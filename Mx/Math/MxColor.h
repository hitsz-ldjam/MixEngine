#pragma once
#ifndef MX_COLOR_H_
#define MX_COLOR_H_

#include "../Math/MxVector4.h"

namespace Mix {
    class Color32;

    class Color {
    public:
        union {
            struct {
                union {
                    struct {
                        float r;
                        float g;
                        float b;
                    };

                    struct {
                        float h;
                        float s;
                        float v;
                    };
                };

                float a;
            };

            float linear[4];
        };

        Color() :Color(0.0f, 0.0f, 0.0f, 1.0f) {}

        Color(float _r, float _g, float _b, float _a) :r(_r), g(_g), b(_b), a(_a) {
        }

        Color(const Vector4f& _vec) :r(_vec.r), g(_vec.g), b(_vec.b), a(_vec.a) {
        }

        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Yellow;
        static const Color Gray;
        static const Color Cyan;
        static const Color Magenta;
        static const Color Clear;

        static float GrayScale(const Color& _color) { return _color.r*0.3f + _color.g*0.59f + _color.b*0.11f; }

        static Color Lerp(const Color& _a, const Color& _b, float _t);

        static Color LerpUnclamped(const Color& _a, const Color& _b, float _t);

        static Color RGBToHSV(const Color& _rgb);

        static Color HSVToRGB(const Color& _hsv);

        float const& operator[](const uint32_t _index) const;

        float& operator[](const uint32_t _index);

        operator Vector4f() const;

        std::string toString() const {
            return (boost::format("Color(%f, %f, %f, %f)") % r % g % b % a).str();
        }
    };

    class Color32 {
    public:
        union {
            struct {
                char r;
                char g;
                char b;
                char a;
            };

            char linear[4];
        };

        Color32() :r(0), g(0), b(0), a(0) {}

        Color32(char _r, char _g, char _b, char _a) :r(_r), g(_g), b(_b), a(_a) {}

        Color32(const Color& _color);

        operator Color() const;

        std::string toString() const {
            return (boost::format("Color32(%d,%d,%d,%d)") % r % g % b % a).str();
        }
    };
}

#endif
