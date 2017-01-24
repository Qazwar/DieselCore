#include "FourierPath.h"

namespace ds {

	FourierPath::FourierPath() {
		num = 0;
	}

	void FourierPath::build() {
		arcLength[0] = 0.0f;
		v2 p;
		v2 previous;
		totalLength = 0.0f;
		for (int i = 0; i <= MAX_FP_STEPS; ++i) {
			float t = (float)i / static_cast<float>(MAX_FP_STEPS);
			get(t, &p);
			if (i > 0) {
				float x_diff = p.x - previous.x;
				double y_diff = p.y - previous.y;
				totalLength += sqrt(x_diff * x_diff + y_diff * y_diff);
				arcLength[i] = totalLength;
			}
			previous = p;
		}
	}

	void FourierPath::approx(float u, v2* p) const {
		if (u >= 0.0f && u <= 1.0f) {
			float t = find(u);
			get(t, p);
		}
	}

	float FourierPath::find(float u) const {
		if (u == 0.0f) {
			return 0.0f;
		}
		float targetLength = u * totalLength;
		int low = 0;
		int high = MAX_FP_STEPS;
		int index = 0;
		while (low < high) {
			index = (low + (high - low) / 2);
			if (arcLength[index] < targetLength) {
				low = index + 1;

			}
			else {
				high = index;
			}
		}
		if (arcLength[index] > targetLength) {
			index--;
		}
		float lengthBefore = arcLength[index];
		if (lengthBefore == targetLength) {
			return lengthBefore;

		}
		else {
			return (index + (targetLength - lengthBefore) / (arcLength[index + 1] - lengthBefore)) / static_cast<float>(MAX_FP_STEPS);
		}
	}

	void FourierPath::get(float time, v2* p) const {
		float t = time * frequency * TWO_PI;
		float v = 0.0f;
		for (int i = 0; i < num; ++i) {
			float f = static_cast<float>(i) * 2.0f + 1.0f;
			v += values[i] / PI * sin(f * t + DEGTORAD(offset));
		}
		p->y = v * height;
		p->x = 1280.0f - 1280.0f * time;
	}

	bool FourierPath::loadData(const JSONReader& loader, int category) {			
		loader.get_int(category, "num", &num);
		loader.get(category, "v1", &values[0]);
		loader.get(category, "v2", &values[1]);
		loader.get(category, "v3", &values[2]);
		loader.get(category, "v4", &values[3]);
		loader.get(category, "v5", &values[4]);
		loader.get(category, "v6", &values[5]);
		loader.get(category, "v7", &values[6]);
		loader.get(category, "v8", &values[7]);
		loader.get(category, "height", &height);
		loader.get(category, "offset", &offset);
		loader.get(category, "intervall", &frequency);
		min = 10000.0f;
		max = -10000.0f;
		build();
		for (int j = 0; j < MAX_FP_STEPS; ++j) {
			float t = static_cast<float>(j) / static_cast<float>(MAX_FP_STEPS);
			v2 p;
			approx(t, &p);
			if (p.y > max) {
				max = p.y;
			}
			if (p.y < min) {
				min = p.y;
			}
		}
		return true;
	}
}