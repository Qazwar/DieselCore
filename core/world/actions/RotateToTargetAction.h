#pragma once
#include "..\..\math\tweening.h"
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	class RotateToTargetAction : public AbstractAction {

	public:
		RotateToTargetAction(ChannelArray* array);
		virtual ~RotateToTargetAction() {}
		void attach(ID id,ID target,float angleVelocity);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_ROTATE_TO_TARGET;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		ID* _targets;
		float* _angleVelocities;
	};

}