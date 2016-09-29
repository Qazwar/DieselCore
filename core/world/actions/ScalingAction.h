#pragma once
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class ScalingAction : public AbstractAction {
		
	public:
		ScalingAction(ChannelArray* array);
		virtual ~ScalingAction();
		void attach(ID id, int channel, const v3& startScale, const v3& endScale,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void update(float dt,ActionEventBuffer& buffer);
		void debug();
		void debug(ID sid) {}
		ActionType getActionType() const {
			return AT_SCALE;
		}
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