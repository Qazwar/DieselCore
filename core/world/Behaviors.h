#pragma once
#include "ActionManager.h"
#include "..\lib\collection_types.h"
//#include "actions\AbstractAction.h"
#include "..\string\StaticHash.h"

namespace ds {

	struct ActionSettings;

	enum BehaviorTrigger {
		BT_TIMER,
		BT_EVENT,
		BT_NONE
	};

	struct Behavior {
		StaticHash hash;
		Array<ActionSettings*> settings;
		float ttl;
		float ttlVariance;
	};

	struct ActiveBehavior {
		float timer;
		float ttl;
		ID reference;
		BehaviorTrigger trigger;
		int objectType; //????
		ActionType actionType;
	};

	struct ActionDefinition {

		ActionSettings* settings;

		ActionDefinition(ActionSettings* s) : settings(s) {}
	};

	struct SettingsDefinition {
		ID behavior;
		ActionSettings* settings;

		SettingsDefinition(ID b, ActionSettings* s) : behavior(b), settings(s) {}
	};

	struct ConnectionDefinition {
		StaticHash from;
		ActionType type;
		StaticHash to;

		ConnectionDefinition(StaticHash f, ActionType at, StaticHash t) : from(f), type(at), to(t) {}
	};

	struct BehaviorTransition {
		ID from;
		ID to;
		ActionType type;
		int objectType;
	};

	class Behaviors {

	public:
		Behaviors(ActionManager* actionManager);
		~Behaviors();
		ID create(const char* name);
		void tick(float dt);
		void addSettings(ID behaviorID, ActionSettings* settings);
		void start(const StaticHash& hash, ID id);
		void connect(ID first, const ActionType& type, ID second, int objectType);
		void connect(StaticHash first, const ActionType& type, StaticHash second, int objectType);
		void processEvent(const ActionEvent& event);
	private:
		void start(int index, ID id);
		ID findTransition(ActionType type, int objectType);
		ActionManager* _actionManager;
		Array<Behavior*> _behaviors;
		Array<BehaviorTransition> _transitions;
		Array<ActiveBehavior> _activeBehaviors;
	};

}

