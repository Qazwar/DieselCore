#pragma once
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"
#include "..\..\physics\ColliderArray.h"

namespace ds {

	class CollisionAction : public AbstractAction {

		struct IgnoredCollision {
			int firstType;
			int secondType;
		};
		
	public:
		CollisionAction(ChannelArray* array);
		virtual ~CollisionAction();
		void ignore(int firstType, int secondType);
		void attach(ID id, ShapeType type, const v3& extent);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_COLLISION;
		}
		void saveReport(const ReportWriter& writer);
		bool hasCollisions() const {
			return !_collisions.empty();
		}
		const Collision& getCollision(int idx) const {
			return _collisions[idx];
		}
		uint32_t numCollisions() const {
			return _collisions.size();
		}
	private:
		bool containsCollision(const Collision& c) const;
		bool intersects(int firstIndex, int secondIndex, Collision* c);
		bool isSupported(int firstType, int secondType);
		void allocate(int sz);
		v3* _previous;
		ShapeType* _types;
		v3* _extents;
		Array<Collision> _collisions;
		Array<IgnoredCollision> _ignores;
	};

}