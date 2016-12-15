#pragma once
#include "..\lib\BlockArray.h"
#include "..\math\math_types.h"
#include "ActionEventBuffer.h"

namespace ds {

	class CollisionAction;
	class AbstractAction;

	const int MAX_ACTIONS = 32;

	class ActionManager {

	public:
		ActionManager(ChannelArray* data, Rect boundingRect);
		~ActionManager();
		void setBoundingRect(const Rect& boundingRect);
		AbstractAction* get(ActionType type);
		void removeByID(ID id);
		void stopAction(ID id, ActionType type);
		bool isActive(ID id, ActionType type);
		void update(float dt, ActionEventBuffer& buffer);
		void saveReport(const ReportWriter& writer);
		CollisionAction* getCollisionAction();
		bool supportCollisions() const;
	private:
		void createAction(ActionType type);
		ChannelArray* _data;
		Rect _boundingRect;
		AbstractAction* _actions[MAX_ACTIONS];
		CollisionAction* _collisionAction;
	};

}