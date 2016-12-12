#pragma once
#include "..\World.h"
#include "AbstractAction.h"
#include "..\..\math\FloatArray.h"

namespace ds {

	struct ScaleByPathActionSettings : public ActionSettings {

		V3Path* path;
		float ttl;
		float ttlVariance;

		ScaleByPathActionSettings() : path(0), ttl(0.0f) , ttlVariance(0.0f) {
			type = AT_SCALE_BY_PATH;
		}
		ScaleByPathActionSettings(V3Path* p, float t) : path(p), ttl(t), ttlVariance(0.0f) {
			type = AT_SCALE_BY_PATH;
		}
		ScaleByPathActionSettings(V3Path* p, float t, float tv) : path(p), ttl(t), ttlVariance(tv) {
			type = AT_SCALE_BY_PATH;
		}

	};

	class ScaleByPathAction : public AbstractAction {

	public:
		ScaleByPathAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~ScaleByPathAction();
		void attach(ID id, V3Path* path, float ttl);
		void attach(ID id, ActionSettings* settings);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_SCALE_BY_PATH;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		V3Path** _path;
		float* _timers;
		float* _ttl;
	};

}