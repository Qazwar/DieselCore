#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	struct SeparateSettings : public ActionSettings {

		int objectType;
		float minDistance;
		float relaxation;

		SeparateSettings() : objectType(-1) , minDistance(0.0f) , relaxation(0.0f) {
			type = AT_SEPARATE;
			ttl = 0.0f;
			ttlVariance = 0.0f;
		}

		SeparateSettings(int o,float d,float r) : objectType(o) , minDistance(d), relaxation(r) {
			type = AT_SEPARATE;
			ttl = 0.0f;
			ttlVariance = 0.0f;
		}

		SeparateSettings(int o, float d, float r, float t) : objectType(o), minDistance(d), relaxation(r) {
			type = AT_SEPARATE;
			ttl = t;
			ttlVariance = 0.0f;
		}


	};

	class SeparateAction : public AbstractAction {

	public:
		SeparateAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~SeparateAction() {}
		void attach(ID id, ActionSettings* settings);
		void attach(ID id, int type, float minDistance, float relaxation);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_SEPARATE;
		}
		void saveReport(const ReportWriter& writer);
	private:
		int find_by_type(int type, ID* ids, int max) const;
		void allocate(int sz);
		int* _types;
		float* _minDistances;
		float* _relaxations;
	};

}