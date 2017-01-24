#pragma once
#include <Vector.h>
#include "..\lib\collection_types.h"
#include "BezierCurve.h"
#include "..\io\json.h"

namespace ds {

	const int MAX_CBP_STEPS = 64;

	class Path {

	public:
		Path() {}
		virtual ~Path() {}
		virtual void build() = 0;
		virtual void get(float t, v2* p) const = 0;
		virtual void approx(float t, v2* p) const = 0;
		virtual void tanget(float u, v2* tangent) const = 0;
		virtual const int size() const = 0;
		virtual bool loadData(const JSONReader& loader, int category) = 0;
	};
	
	class CubicBezierPath : public Path {

	typedef Array<BezierCurve> BezierElements;

	public:
		CubicBezierPath();
		~CubicBezierPath();
		void create(const v2& p0,const v2& p1,const v2& p2,const v2& p3);
		void add(const v2& p1,const v2& p2,const v2& p3);	
		void build();
		void get(float t,v2* p) const;
		void approx(float u, v2* p) const;
		void tanget(float u, v2* tangent) const;
		const int size() const {
			return m_Elements.size();
		}
		const BezierCurve& getElement(int idx) const {
			return m_Elements[idx];
		}
		bool loadData(const JSONReader& loader, int category);
	private:
		float find(float u) const;
		BezierElements m_Elements;
		float m_TotalLength;
		float m_ArcLength[MAX_CBP_STEPS+1];
	};

}