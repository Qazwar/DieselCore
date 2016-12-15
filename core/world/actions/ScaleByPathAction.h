#pragma once
#include "..\World.h"
#include "AbstractAction.h"
#include "..\..\math\FloatArray.h"

namespace ds {

	struct ScaleByPathActionSettings : public ActionSettings {

		V3Path* path;
		
		ScaleByPathActionSettings() : path(0) {
			type = AT_SCALE_BY_PATH;
			ttl = 0.0f;
			ttlVariance = 0.0f;
		}
		ScaleByPathActionSettings(V3Path* p, float t) : path(p) {
			type = AT_SCALE_BY_PATH;
			ttl = t;
			ttlVariance = 0.0f;
		}
		ScaleByPathActionSettings(V3Path* p, float t, float tv) : path(p) {
			type = AT_SCALE_BY_PATH;
			ttl = t;
			ttlVariance = tv;
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