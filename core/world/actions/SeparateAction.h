#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	class SeparateAction : public AbstractAction {

	public:
		SeparateAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~SeparateAction() {}
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