#pragma once
#include "..\..\math\tweening.h"
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	struct RotateBySettings : public ActionSettings {

		float angle;
		float angleVariance;

		RotateBySettings() : angle(0.0f) , angleVariance(0.0f) {
			type = AT_ROTATE_BY;
			ttl = 0.0f;
			ttlVariance = 0.0f;
		}
		RotateBySettings(float a) : angle(a) , angleVariance(0.0f) {
			type = AT_ROTATE_BY;
			ttl = 0.0f;
			ttlVariance = 0.0f;
		}
		RotateBySettings(float a, float t) : angle(a), angleVariance(0.0f) {
			type = AT_ROTATE_BY;
			ttl = t;
			ttlVariance = 0.0f;
		}
		RotateBySettings(float a, float t, float tv) : angle(a) {
			type = AT_ROTATE_BY;
			ttl = t;
			ttlVariance = tv;
		}

	};

	class RotateByAction : public AbstractAction {

	public:
		RotateByAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~RotateByAction() {}
		void attach(ID id, ActionSettings* settings);
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