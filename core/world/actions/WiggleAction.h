#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	struct WiggleActionSettings : public ActionSettings {

		float amplitude;
		float frequency;

		WiggleActionSettings() : amplitude(0.0f), frequency(0.0f) {
			type = AT_WIGGLE;
			ttl = 0.0f;
			ttlVariance = 0.0f;
		}

		WiggleActionSettings(float a,float f) : amplitude(a), frequency(f) {
			type = AT_WIGGLE;
			ttl = 0.0f;
			ttlVariance = 0.0f;
		}

		WiggleActionSettings(float a, float f, float t) : amplitude(a), frequency(f) {
			type = AT_WIGGLE;
			ttl = t;
			ttlVariance = 0.0f;
		}
		

	};

	class WiggleAction : public AbstractAction {

	public:
		WiggleAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~WiggleAction() {}
		void attach(ID id, ActionSettings* settings);
		void attach(ID id,float amplitude, float frequency ,float ttl = -1.0f);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_WIGGLE;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);

		float* _amplitudes;
		float* _frequencies;
		float* _timers;
		float* _ttl;
	};

}