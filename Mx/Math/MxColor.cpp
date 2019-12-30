#include "MxColor.h"

namespace Mix {
    const Color Color::Black = Color(0.0f, 0.0f, 0.0f, 1.0f);
    const Color Color::White = Color(1.0f, 1.0f, 1.0f, 1.0f);
    const Color Color::Red = Color(1.0f, 0.0f, 0.0f, 1.0f);
    const Color Color::Green = Color(0.0f, 1.0f, 0.0f, 1.0f);
    const Color Color::Blue = Color(0.0f, 0.0f, 1.0f, 1.0f);
    const Color Color::Yellow = Color(1.0f, 0.92f, 0.016f, 1.0f);
    const Color Color::Gray = Color(0.5f, 0.5f, 0.5f, 1.0f);
    const Color Color::Cyan = Color(0.0f, 1.0f, 1.0f, 1.0f);
    const Color Color::Magenta = Color(1.0f, 0.0f, 1.0f, 1.0f);
    const Color Color::Clear = Color(0.0f, 0.0f, 0.0f, 0.0f);

    Color Color::Lerp(const Color& _a, const Color& _b, float _t) {
        _t = std::clamp(_t, 0.0f, 1.0f);
        return Color(_b.r * _t + _a.r * (1.0f - _t),
                     _b.g * _t + _a.g * (1.0f - _t),
                     _b.b * _t + _a.b * (1.0f - _t),
                     _b.a * _t + _a.a * (1.0f - _t));
    }

    Color Color::LerpUnclamped(const Color& _a, const Color& _b, float _t) {
        return Color(_b.r * _t + _a.r * (1.0f - _t),
                     _b.g * _t + _a.g * (1.0f - _t),
                     _b.b * _t + _a.b * (1.0f - _t),
                     _b.a * _t + _a.a * (1.0f - _t));
    }

    Color Color::RGBToHSV(const Color& _rgb) {
        Color         out;
        float      min, max, delta;

        min = _rgb.r < _rgb.g ? _rgb.r : _rgb.g;
        min = min < _rgb.b ? min : _rgb.b;

        max = _rgb.r > _rgb.g ? _rgb.r : _rgb.g;
        max = max > _rgb.b ? max : _rgb.b;

        out.v = max;                                // v
        delta = max - min;
        if (delta < 0.00001) {
            out.s = 0;
            out.h = 0; // undefined, maybe nan?
            return out;
        }
        if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
            out.s = (delta / max);                  // s
        }
        else {
         // if max is 0, then r = g = b = 0              
         // s = 0, h is undefined
            out.s = 0.0;
            out.h = NAN;                            // its now undefined
            return out;
        }
        if (_rgb.r >= max)                           // > is bogus, just keeps compilor happy
            out.h = (_rgb.g - _rgb.b) / delta;        // between yellow & magenta
        else
            if (_rgb.g >= max)
                out.h = 2.0 + (_rgb.b - _rgb.r) / delta;  // between cyan & yellow
            else
                out.h = 4.0 + (_rgb.r - _rgb.g) / delta;  // between magenta & cyan

        out.h *= 60.0;                              // degrees

        if (out.h < 0.0)
            out.h += 360.0;

        return out;
    }



    Color Color::HSVToRGB(const Color& _hsv) {
        double      hh, p, q, t, ff;
        long        i;
        Color         out;

        if (_hsv.s <= 0.0) {       // < is bogus, just shuts up warnings
            out.r = _hsv.v;
            out.g = _hsv.v;
            out.b = _hsv.v;
            return out;
        }
        hh = _hsv.h;
        if (hh >= 360.0) hh = 0.0;
        hh /= 60.0;
        i = static_cast<long>(hh);
        ff = hh - i;
        p = _hsv.v * (1.0 - _hsv.s);
        q = _hsv.v * (1.0 - (_hsv.s * ff));
        t = _hsv.v * (1.0 - (_hsv.s * (1.0 - ff)));

        switch (i) {
        case 0:
            out.r = _hsv.v;
            out.g = t;
            out.b = p;
            break;
        case 1:
            out.r = q;
            out.g = _hsv.v;
            out.b = p;
            break;
        case 2:
            out.r = p;
            out.g = _hsv.v;
            out.b = t;
            break;

        case 3:
            out.r = p;
            out.g = q;
            out.b = _hsv.v;
            break;
        case 4:
            out.r = t;
            out.g = p;
            out.b = _hsv.v;
            break;
        case 5:
        default:
            out.r = _hsv.v;
            out.g = p;
            out.b = q;
            break;
        }
        return out;
    }

    float const& Color::operator[](const uint32_t _index) const {
        switch (_index) {
        case 0:
            return r;
        case 1:
            return g;
        case 2:
            return b;
        case 3:
            return a;
        default:
            throw Exception("Color index out of bounds!");
        }
    }

    float& Color::operator[](const uint32_t _index) {
        return const_cast<float&>(static_cast<const Color&>(*this)[_index]);
    }

    Color32::Color32(const Color& _color) {
        r = static_cast<char>(_color.r * 255);
        g = static_cast<char>(_color.g * 255);
        b = static_cast<char>(_color.b * 255);
        a = static_cast<char>(_color.a * 255);
    }

    Color32::operator Color() const {
        return Color(static_cast<float>(r) / 255,
                     static_cast<float>(g) / 255,
                     static_cast<float>(b) / 255,
                     static_cast<float>(a) / 255);
    }
}

