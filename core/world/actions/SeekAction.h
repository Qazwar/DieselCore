#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	struct SeekSettings : public ActionSettings {

		StaticHash target;
		float velocity;

		SeekSettings() : velocity(0.0f) {
			target = SID("-");
			type = AT_SEEK;
			ttl = 0.0f;
			ttlVariance = 0.0f;
		}

		SeekSettings(StaticHash h, float v) : target(h) , velocity(v) {
			type = AT_SEEK;
			ttl = 0.0f;
			ttlVariance = 0.0f;
		}
	};

	class SeekAction : public AbstractAction {

	public:
		SeekAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~SeekAction() {}
		void attach(ID id, ActionSettings* settings);
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