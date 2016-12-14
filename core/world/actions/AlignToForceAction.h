#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	struct AlignToForceActionSettings : public ActionSettings {

		float ttl;
		float ttlVariance;

		AlignToForceActionSettings() : ttl(0.0f), ttlVariance(0.0f) {
			type = AT_ALIGN_TO_FORCE;
		}

		AlignToForceActionSettings(float t) : ttl(t), ttlVariance(0.0f) {
			type = AT_ALIGN_TO_FORCE;
		}
		

	};

	class AlignToForceAction : public AbstractAction {

	public:
		AlignToForceAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~AlignToForceAction() {}
		void attach(ID id, ActionSettings* settings);
		void attach(ID id, float ttl = -1.0f);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_ALIGN_TO_FORCE;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		float* _timers;
		float* _ttl;
	};

}