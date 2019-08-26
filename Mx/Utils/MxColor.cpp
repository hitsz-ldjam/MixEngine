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

	Color::operator Math::Vector4<float>() const {
		return Math::Vector4f(r, g, b, a);
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
