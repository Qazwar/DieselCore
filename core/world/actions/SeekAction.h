#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	class SeekAction : public AbstractAction {

	public:
		SeekAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~SeekAction() {}
		void attach(ID id, ID target, float velocity);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_SEEK;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		ID* _targets;
		float* _velocities;
	};

}