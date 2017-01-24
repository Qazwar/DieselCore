#pragma once
#include "CubicBezierPath.h"

namespace ds {

	const int MAX_FP_STEPS = 128;

	class FourierPath : public Path {

	public:
		FourierPath();
		virtual ~FourierPath() {}
		void build();
		void approx(float u, v2* p) const;
		float find(float u) const;
		void get(float time, v2* p) const;
		bool loadData(const JSONReader& loader, int category);
		void tanget(float u, v2* tangent) const {}
		const int size() const {
			return num;
		}
	private:
		int num;
		float values[8];
		float height;
		float frequency;
		float min;
		float max;
		float offset;
		float totalLength;
		float arcLength[MAX_FP_STEPS + 1];
	};


}
