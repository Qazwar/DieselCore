#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {
	
	class ColorFlashAction : public AbstractAction {

	public:
		ColorFlashAction(ChannelArray* array, const Rect& boundingRect);
		~ColorFlashAction();
		void attach(ID id,const Color& startColor,const Color& endColor,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void update(float dt,ActionEventBuffer& buffer);		
		void debug();
		void debug(SID sid) {}
		ActionType getActionType() const {
			return AT_COLOR_FLASH;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		Color* _startColors;
		Color* _endColors;
		float* _timers;
		float* _ttl;
		tweening::TweeningType* _tweeningTypes;
		int* _modes;
	};


}