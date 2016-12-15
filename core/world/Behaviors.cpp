#include "Behaviors.h"
#include "..\base\Assert.h"
#include "actions\AbstractAction.h"

namespace ds {

	Behaviors::Behaviors(ActionManager* actionManager) : _actionManager(actionManager) {
	}


	Behaviors::~Behaviors() {
		for (uint32_t i = 0; i < _behaviors.size(); ++i) {
			_behaviors[i]->settings.destroy_all();
		}
	}

	// -----------------------------------------------
	// create behavior
	// -----------------------------------------------
	ID Behaviors::create(const char* name) {
		Behavior* b = new Behavior;
		b->hash = SID(name);
		_behaviors.push_back(b);
		return _behaviors.size() - 1;
	}

	// -----------------------------------------------
	// add settings to behavior
	// -----------------------------------------------
	void Behaviors::addSettings(ID behaviorID, ActionSettings* settings) {
		Behavior* b = _behaviors[behaviorID];
		b->settings.push_back(settings);
	}

	// -----------------------------------------------
	// start behavior
	// -----------------------------------------------
	void Behaviors::start(const StaticHash& hash, ID id) {
		int idx = -1;
		for (uint32_t i = 0; i < _behaviors.size(); ++i) {
			if (_behaviors[i]->hash == hash) {
				idx = i;
			}
		}
		XASSERT(idx != -1, "Cannot find matching behavior");
		start(idx, id);
	}

	// -----------------------------------------------
	// tick
	// -----------------------------------------------
	void Behaviors::tick(float dt) {
		Array<ActiveBehavior>::iterator it = _activeBehaviors.begin();
		while (it != _activeBehaviors.end()) {
			if (it->trigger == BT_TIMER) {
				it->timer += dt;
				if (it->timer >= it->ttl) {
					int tid = findTransition(it->actionType, it->objectType);
					if (tid != -1) {
						const BehaviorTransition& t = _transitions[tid];
						start(t.to, it->reference);
					}
					it = _activeBehaviors.remove(it);
				}
				else {
					++it;
				}
			}
			else {
				++it;
			}
		}
	}

	// -----------------------------------------------
	// start behavior by index
	// -----------------------------------------------
	void Behaviors::start(int idx, ID id) {
		if (idx != -1) {
			Behavior* b = _behaviors[idx];
			for (uint32_t i = 0; i < b->settings.size(); ++i) {
				ActionSettings* s = b->settings[i];
				AbstractAction* action = _actionManager->get(s->type);
				action->attach(id, s);
			}
			ActiveBehavior active;
			active.reference = id;
			active.timer = 0.0f;
			active.trigger = BT_NONE;
			active.ttl = b->ttl;
			_activeBehaviors.push_back(active);
		}
	}

	// -----------------------------------------------
	// connect behaviors
	// -----------------------------------------------
	void Behaviors::connect(StaticHash first, const ActionType& type, StaticHash second, int objectType) {
		BehaviorTransition transition;
		ID from = INVALID_ID;
		ID to = INVALID_ID;
		for (uint32_t i = 0; i < _behaviors.size(); ++i) {
			if (_behaviors[i]->hash == first) {
				from = i;
			}
			if (_behaviors[i]->hash == second) {
				to = i;
			}
		}
		transition.from = from;
		transition.to = to;
		transition.type = type;
		transition.objectType = objectType;
		_transitions.push_back(transition);
	}

	// -----------------------------------------------
	// connect behaviors
	// -----------------------------------------------
	void Behaviors::connect(ID first, const ActionType& type, ID second, int objectType) {
		BehaviorTransition transition;
		transition.from = first;
		transition.to = second;
		transition.type = type;
		transition.objectType = objectType;
		_transitions.push_back(transition);
	}

	// -----------------------------------------------
	// find transition
	// -----------------------------------------------
	ID Behaviors::findTransition(ActionType type, int objectType) {
		for (uint32_t i = 0; i < _transitions.size(); ++i) {
			const BehaviorTransition& t = _transitions[i];
			if (t.type == type && t.objectType == objectType) {
				return i;
			}
		}
		return -1;
	}

	// -----------------------------------------------
	// process event
	// -----------------------------------------------
	void Behaviors::processEvent(const ActionEvent& event) {
		int tid = findTransition(event.action, event.type);
		if (tid != -1) {
			const BehaviorTransition& t = _transitions[tid];
			start(t.to, event.id);
		}
	}
}