#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	struct LookAtActionSettings : public ActionSettings {

		StaticHash target;
		float ttl;

		LookAtActionSettings(StaticHash h, float t) : target(h), ttl(t) {
			type = AT_LOOK_AT;
		}
	};

	class LookAtAction : public AbstractAction {

	public:
		LookAtAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~LookAtAction() {}
		void attach(ID id, ID target, float ttl = -1.0f);
		void attach(ID id, ActionSettings* settings);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_LOOK_AT;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		ID* _targets;
		float* _timers;
		float* _ttl;
	};

}