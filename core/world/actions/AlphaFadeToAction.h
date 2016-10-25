#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	class AlphaFadeToAction : public AbstractAction {

	public:
		AlphaFadeToAction(ChannelArray* array, const Rect& boundingRect);
		~AlphaFadeToAction();
		void attach(ID id,float startAlpha,float endAlpha,float ttl);
		void update(float dt, ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_ALPHA_FADE_TO;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		float* _startAlphas;
		float* _endAlphas;
		float* _timers;
		float* _ttl;
	};


}