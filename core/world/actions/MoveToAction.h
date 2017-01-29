#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	struct MoveToSettings : public ActionSettings {

		v3 velocity;
		float radialVelocity;
		bool bounce;

		MoveToSettings() : velocity(0.0f), radialVelocity(0.0f), bounce(false) {
			type = AT_MOVE_TO;
			ttl = 0.0f;
			ttlVariance = 0.0f;
		}
		MoveToSettings(const v3& vel,float t) : velocity(vel), radialVelocity(0.0f), bounce(false) {
			type = AT_MOVE_TO;
			ttl = t;
			ttlVariance = 0.0f;
		}
		MoveToSettings(float vel, float t) : velocity(0.0f), radialVelocity(vel), bounce(false) {
			type = AT_MOVE_TO;
			ttl = t;
			ttlVariance = 0.0f;
		}
		MoveToSettings(float vel, float t,bool b) : velocity(0.0f), radialVelocity(vel), bounce(b) {
			type = AT_MOVE_TO;
			ttl = t;
			ttlVariance = 0.0f;
		}

	};

	class MoveToAction : public AbstractAction {

	public:
		MoveToAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~MoveToAction() {}
		void attach(ID id, ActionSettings* settings);
		void attach(ID id,const v3& start, const v3& end, float ttl, const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void update(float dt,ActionEventBuffer& buffer);
		ActionType getActionType() const {
			return AT_MOVE_TO;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		void rotateTo(int index);
		int isOutOfBounds(const v3& pos, const v3& v,const v2& dim);

		v3* _start;
		v3* _end;
		tweening::TweeningType* _tweeningTypes;
		float* _timers;
		float* _ttl;
	};

}