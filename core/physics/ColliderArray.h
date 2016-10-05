#pragma once
#include <Vector.h>
#include "..\math\AABBox.h"
#include "..\Common.h"

namespace ds {

	struct Collision {

		v3 firstPos;
		v3 secondPos;
		ID firstID;
		ID secondID;
		int firstType;
		int secondType;
		v3 norm;
		float distance;

		const bool containsType(int type) const {
			if ( firstType == type ) {
				return true;
			}
			if ( secondType == type ) {
				return true;
			}
			return false;
		}

		ID getIDByType(int type) const {
			if (firstType == type) {
				return firstID;
			}
			return secondID;
		}

		const v3& getPositionByType(int type) const {
			if (type == firstType) {
				return firstPos;
			}
			return secondPos;
		}

		bool isBetween(int first, int second) const {
			if (firstType == first && secondType == second) {
				return true;
			}
			if (firstType == second && secondType == first) {
				return true;
			}
			return false;
		}

	};

	namespace physics {

		void debug(const Collision& c);

		// ------------------------------------------------------------------
		// test if a line from A to B intersects with a line from C to D
		// ------------------------------------------------------------------
		bool testLineIntersections(const v2& a, const v2& b, const v2& c, const v2& d);

		int testLineBox(const v2& lineStart, const v2& boxCenter, const v2& dim);

		bool isInsideBox(const Vector2f& ray,const Vector2f& p,const Vector2f& dim);

		bool testCircleSweepIntersection(float ra, const Vector2f& A0, const Vector2f& A1, float rb, const Vector2f& B0, const Vector2f&	B1, float* u0, float* u1);

		bool testCircleIntersection(const Vector2f& p1, float r1, const Vector2f& p2, float r2);

		bool testBoxIntersection(const Vector2f& p1,const Vector2f& e1,const Vector2f& p2,const Vector2f& e2);

		bool testBoxIntersection(const Vector3f& p1, const Vector3f& e1, const Vector3f& p2, const Vector3f& e2);

		bool sweepTest(const AABBox& b1, const Vector3f& prev1, const AABBox& b2, const Vector3f& prev2, float* u0, float *u1);

		bool testStaticAABBAABB(AABBox a, AABBox b, Vector3f* normal, float* penetration);
		
	}

}
