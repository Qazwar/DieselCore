#include "ActionManager.h"
#include "actions\ScalingAction.h"
#include "actions\RemoveAfterAction.h"
#include "actions\ScaleByPathAction.h"
#include "actions\MoveByAction.h"
#include "actions\MoveToAction.h"
#include "actions\RotateByAction.h"
#include "actions\RotateAction.h"
#include "actions\RotateToTargetAction.h"
#include "actions\LookAtAction.h"
#include "actions\SeekAction.h"
#include "actions\SeparateAction.h"
#include "..\profiler\Profiler.h"
#include "actions\CollisionAction.h"
#include "actions\AlphaFadeToAction.h"
#include "actions\ScaleAxesAction.h"
#include "actions\ColorFlashAction.h"
#include "actions\WiggleAction.h"
#include "actions\AlignToForceAction.h"
#include "actions\CollisionAction.h"

namespace ds {

	ActionManager::ActionManager(ChannelArray* data, Rect boundingRect) : _data(data) , _boundingRect(boundingRect) {
		_collisionAction = 0;
		for (int i = 0; i < MAX_ACTIONS; ++i) {
			_actions[i] = 0;
		}
	}


	ActionManager::~ActionManager() {
		for (int i = 0; i < MAX_ACTIONS; ++i) {
			if (_actions[i] != 0) {
				delete _actions[i];
			}
		}
		if (_collisionAction != 0) {
			delete _collisionAction;
		}
	}

	void ActionManager::setBoundingRect(const Rect& boundingRect) {
		_boundingRect = boundingRect;
	}

	AbstractAction* ActionManager::get(ActionType type) {
		if (type == AT_COLLISION) {
			return getCollisionAction();
		}
		if (_actions[type] == 0) {
			createAction(type);
		}
		return _actions[type];
	}

	void ActionManager::removeByID(ID id) {
		for (int i = 0; i < MAX_ACTIONS; ++i) {
			if (_actions[i] != 0) {
				_actions[i]->removeByID(id);
			}
		}
	}

	void ActionManager::stopAction(ID id, ActionType type) {
		if (_actions[type] != 0) {
			_actions[type]->removeByID(id);
		}
	}

	bool ActionManager::supportCollisions() const {
		return _collisionAction != 0;
	}

	// -----------------------------------------------
	// is active
	// -----------------------------------------------
	bool ActionManager::isActive(ID id, ActionType type) {
		if (_actions[type] != 0) {
			return _actions[type]->contains(id);
		}
		return false;
	}

	void ActionManager::update(float dt, ActionEventBuffer& buffer) {
		ZoneTracker u1("World::tick::update");
		for (int i = 0; i < MAX_ACTIONS; ++i) {
			if (_actions[i] != 0) {
				_actions[i]->update(dt, buffer);
			}
		}
	}

	CollisionAction* ActionManager::getCollisionAction() {
		if (_collisionAction == 0) {
			_collisionAction = new CollisionAction(_data, _boundingRect);
		}
		return _collisionAction;
	}

	void ActionManager::createAction(ActionType type) {
		if (_actions[type] == 0) {
			switch (type) {
			case AT_SCALE_BY_PATH:_actions[AT_SCALE_BY_PATH] = new ScaleByPathAction(_data, _boundingRect); break;
			case AT_SCALE_AXES: _actions[AT_SCALE_AXES] = new ScaleAxesAction(_data, _boundingRect); break;
			case AT_SCALE: _actions[AT_SCALE] = new ScalingAction(_data, _boundingRect); break;
			case AT_COLLISION: _collisionAction = new CollisionAction(_data, _boundingRect); break;
			case AT_MOVE_BY: _actions[AT_MOVE_BY] = new MoveByAction(_data, _boundingRect); break;
			case AT_MOVE_TO: _actions[AT_MOVE_TO] = new MoveToAction(_data, _boundingRect); break;
			case AT_REMOVE_AFTER: _actions[AT_REMOVE_AFTER] = new RemoveAfterAction(_data, _boundingRect); break;
			case AT_COLOR_FLASH: _actions[AT_COLOR_FLASH] = new ColorFlashAction(_data, _boundingRect); break;
			case AT_ALPHA_FADE_TO: _actions[AT_ALPHA_FADE_TO] = new AlphaFadeToAction(_data, _boundingRect); break;
			case AT_SEPARATE: _actions[AT_SEPARATE] = new SeparateAction(_data, _boundingRect); break;
			case AT_SEEK: _actions[AT_SEEK] = new SeekAction(_data, _boundingRect); break;
			case AT_LOOK_AT: _actions[AT_LOOK_AT] = new LookAtAction(_data, _boundingRect); break;
			case AT_ROTATE_TO_TARGET: _actions[AT_ROTATE_TO_TARGET] = new RotateToTargetAction(_data, _boundingRect); break;
			case AT_ROTATE: _actions[AT_ROTATE] = new RotateAction(_data, _boundingRect); break;
			case AT_ROTATE_BY: _actions[AT_ROTATE_BY] = new RotateByAction(_data, _boundingRect); break;
			case AT_WIGGLE: _actions[AT_WIGGLE] = new WiggleAction(_data, _boundingRect); break;
			case AT_ALIGN_TO_FORCE: _actions[AT_ALIGN_TO_FORCE] = new AlignToForceAction(_data, _boundingRect); break;
			}
		}
	}

	void ActionManager::saveReport(const ReportWriter& writer) {
		for (int i = 0; i < MAX_ACTIONS; ++i) {
			if (_actions[i] != 0) {
				_actions[i]->saveReport(writer);
			}
		}
	}
}