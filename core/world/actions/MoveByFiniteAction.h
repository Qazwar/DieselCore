#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	class MoveByFiniteAction : public AbstractAction {

	public:
		MoveByFiniteAction(ChannelArray* array);
		virtual ~MoveByFiniteAction() {}
		void attach(ID id,const v3& velocity,float ttl, bool bounce = true);
		void update(float dt,ActionEventBuffer& buffer);
		void bounce(ID sid, BounceDirection direction,float dt);
		ActionType getActionType() const {
			return AT_MOVE_BY_FINITE;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		void rotateTo(int index);
		bool isOutOfBounds(const v3& pos, const v3& v);

		v3* _velocities;
		float* _timers;
		float* _ttl;
		bool* _bounce;
	};

}