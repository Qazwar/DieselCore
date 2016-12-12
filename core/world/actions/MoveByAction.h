#pragma once
#include "..\World.h"
#include "AbstractAction.h"

namespace ds {

	struct MoveByActionSettings : public ActionSettings {

		v3 velocity;
		float radialVelocity;
		float ttl;
		float ttlVariance;
		bool bounce;

		MoveByActionSettings() : velocity(0.0f), radialVelocity(0.0f), ttl(0.0f), ttlVariance(0.0f), bounce(false) {
			type = AT_MOVE_BY;
		}
		MoveByActionSettings(const v3& vel,float t) : velocity(vel), radialVelocity(0.0f), ttl(t), ttlVariance(0.0f), bounce(false) {
			type = AT_MOVE_BY;
		}
		MoveByActionSettings(float vel, float t) : velocity(0.0f), radialVelocity(vel), ttl(t), ttlVariance(0.0f), bounce(false) {
			type = AT_MOVE_BY;
		}
		MoveByActionSettings(float vel, float t,bool b) : velocity(0.0f), radialVelocity(vel), ttl(t), ttlVariance(0.0f), bounce(b) {
			type = AT_MOVE_BY;
		}

	};

	class MoveByAction : public AbstractAction {

	public:
		MoveByAction(ChannelArray* array, const Rect& boundingRect);
		virtual ~MoveByAction() {}
		void attach(ID id, ActionSettings* settings);
		void attach(ID id,const v3& velocity,float ttl = -1.0f, bool bounce = true);
		void update(float dt,ActionEventBuffer& buffer);
		void bounce(ID sid, BounceDirection direction,float dt);
		ActionType getActionType() const {
			return AT_MOVE_BY;
		}
		void saveReport(const ReportWriter& writer);
	private:
		void allocate(int sz);
		void rotateTo(int index);
		int isOutOfBounds(const v3& pos, const v3& v,const v2& dim);

		v3* _velocities;
		float* _timers;
		float* _ttl;
		bool* _bounce;
	};

}