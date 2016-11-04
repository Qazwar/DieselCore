#pragma once
#include "..\..\math\tweening.h"
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	class RotateAction : public AbstractAction {

	public:
		RotateAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~RotateAction() {}
		void attach(ID id,const v3& velocity, float ttl);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_ROTATE;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		v3* _velocities;
		float* _timers;
		float* _ttl;
	};

}