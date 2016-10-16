#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	class MoveByAction : public AbstractAction {

	public:
		MoveByAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~MoveByAction() {}
		void attach(ID id,const v3& velocity,float ttl = -1.0f, bool bounce = true);
		void update(float dt,ActionEventBuffer& buffer);
		void bounce(ID sid, BounceDirection direction,float dt);
		ActionType getActionType() const {
			return AT_MOVE_BY;
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