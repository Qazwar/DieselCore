#pragma once
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class ScalingAction : public AbstractAction {
		
	public:
		ScalingAction(MultiplexArray* array);
		virtual ~ScalingAction();
		void attach(ID id, int channel, const v4& startScale, const v4& endScale,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void update(float dt,ActionEventBuffer& buffer);
		void debug();
		void debug(ID sid) {}
		ActionType getActionType() const {
			return AT_SCALE;
		}
	private:
		void allocate(int sz);
		v4* _startScale;
		v4* _endScale;
		float* _timers;
		float* _ttl;
		tweening::TweeningType* _tweeningTypes;
		int* _modes;
	};

}