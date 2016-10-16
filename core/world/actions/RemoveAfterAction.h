#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	class RemoveAfterAction : public AbstractAction {

	public:
		RemoveAfterAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~RemoveAfterAction() {}
		void attach(ID id, float ttl);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_REMOVE_AFTER;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		float* _timers;
		float* _ttl;
	};

}