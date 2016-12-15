#pragma once
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	struct ScaleSettings : public ActionSettings {

		v3 startScale;
		v3 endScale;
		tweening::TweeningType tweeningType;

		ScaleSettings() : startScale(1.0f), endScale(1.0f) {
			type = AT_SCALE;
			ttl = 0.0f;
			ttlVariance = 0.0f;
		}
		ScaleSettings(const v3& s,const v3& e,float t) : startScale(s), endScale(e) {
			type = AT_SCALE;
			ttl = t;
			ttlVariance = 0.0f;
		}

	};

	class ScalingAction : public AbstractAction {
		
	public:
		ScalingAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~ScalingAction();
		void attach(ID id, int channel, const v3& startScale, const v3& endScale,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_SCALE;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		v3* _startScale;
		v3* _endScale;
		float* _timers;
		float* _ttl;
		tweening::TweeningType* _tweeningTypes;
		int* _modes;
	};

}