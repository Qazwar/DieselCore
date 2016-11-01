#pragma once
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class ScaleAxesAction : public AbstractAction {
		
	public:
		ScaleAxesAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~ScaleAxesAction();
		void attach(ID id, int axes, float start, float end,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_SCALE_AXES;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		float* _startScale;
		float* _endScale;
		int* _axes;
		float* _timers;
		float* _ttl;
		tweening::TweeningType* _tweeningTypes;
		int* _modes;
	};

}