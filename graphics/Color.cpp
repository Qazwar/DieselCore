#include "Color.h"
#include <algorithm>
#include "..\math\math.h"

const ds::Color ds::Color::WHITE(1.0f, 1.0f, 1.0f, 1.0f);

ds::Color operator*=(ds::Color lhs, const ds::Color& rhs) {
	for (int i = 0; i < 4; ++i) {
		lhs.values[i] *= rhs.values[i];
	}
	return lhs;
}

ds::Color operator*(ds::Color lhs, const ds::Color& rhs) {
	return lhs *= rhs;
}

namespace ds {

	namespace color {

		float getColorComponent(float temp1, float temp2, float temp3) {
			if (temp3 < 0.0f)
				temp3 += 1.0f;
			else if (temp3 > 1.0f)
				temp3 -= 1.0f;

			if (temp3 < 1.0f / 6.0f)
				return temp1 + (temp2 - temp1) * 6.0f * temp3;
			else if (temp3 < 0.5f)
				return temp2;
			else if (temp3 < 2.0f / 3.0f)
				return temp1 + ((temp2 - temp1) * ((2.0f / 3.0f) - temp3) * 6.0f);
			else
				return temp1;
		}

		Color hsl2rgb(const HSL& hsl) {
			float r = 0.0f, g = 0.0f, b = 0.0f;
			float l = hsl.l / 100.0f;
			float s = hsl.s / 100.0f;
			float h = hsl.h / 360.0f;
			if (l != 0.0f)	{
				if (s == 0.0f) {
					return Color::WHITE;
				}
				else {
					float temp2;
					if (l < 0.5)
						temp2 = l * (1.0f + s);
					else
						temp2 = l + s - (l * s);

					double temp1 = 2.0f * l - temp2;

					r = getColorComponent(temp1, temp2, h + 1.0f / 3.0f);
					g = getColorComponent(temp1, temp2, h);
					b = getColorComponent(temp1, temp2, h - 1.0f / 3.0f);
				}
			}
			return Color(r, g, b, 1.0f);
		}

		void generateGoldenRatioRainbow(int count, float s, float l, Color* colors, int num) {
			// prepare colors
			float goldenRatioConjugate = 0.618033988749895f;
			float currentHue = math::random(0.0f, 1.0f);
			for (int i = 0; i < num; i++) {
				ds::HSL hslColor = ds::HSL(currentHue * 360.0f, s, l);
				colors[i] = hsl2rgb(hslColor);
				currentHue += goldenRatioConjugate;
				if (currentHue > 1.0f) {
					currentHue -= 1.0f;
				}
			}
		}

		Color lerp(const Color& lhs, const Color& rhs, float t) {
			if (t <= 0.0f) {
				return lhs;
			}
			if (t >= 1.0f) {
				return rhs;
			}
			float invT = 1.0f - t;
			return Color(lhs.r * invT + rhs.r * t, lhs.g * invT + rhs.g * t, lhs.b * invT + rhs.b * t, lhs.a * invT + rhs.a * t);
		}

		// 0 < h < 360 / 0 < s < 100 / 0 < v < 100
		Color hsvToColor(float h, float s, float v) {
			if (h == 0 && s == 0) {
				return Color(v / 100.0f, v / 100.0f, v / 100.0f);
			}

			float hh = h / 60.0f;

			float c = s / 100.0f * v / 100.0f;
			float x = c * (1.0f - std::abs(fmod(hh, 2.0f) - 1.0f));
			Color clr;
			if (hh < 1.0f) {
				clr = Color(c, x, 0.0f);
			}
			else if (hh < 2.0f) {
				clr = Color(x, c, 0.0f);
			}
			else if (hh < 3.0f) {
				clr = Color(0.0f, c, x);
			}
			else if (hh < 4.0f) {
				clr = Color(0.0f, x, c);
			}
			else if (hh < 5) {
				clr = Color(x, 0.0f, c);
			}
			else {
				clr = Color(c, 0.0f, x);
			}
			float m = v / 100.0f - c;
			clr.r += m;
			clr.g += m;
			clr.b += m;
			return clr;
		}
	}

}