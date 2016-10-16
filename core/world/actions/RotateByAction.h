#pragma once
#include "..\..\math\tweening.h"
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	class RotateByAction : public AbstractAction {

	public:
		RotateByAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~RotateByAction() {}
		void attach(ID id,float angle, float ttl);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_ROTATE_BY;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		float* _angles;
		float* _timers;
		float* _ttl;
	};

}