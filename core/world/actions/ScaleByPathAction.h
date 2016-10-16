#pragma once
#include "..\World.h"
#include "AbstractAction.h"
#include "..\..\math\FloatArray.h"

namespace ds {

	class ScaleByPathAction : public AbstractAction {

	public:
		ScaleByPathAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~ScaleByPathAction();
		void attach(ID id, V3Path* path, float ttl);
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