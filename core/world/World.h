#pragma once
#include "..\lib\BlockArray.h"
#include "..\lib\collection_types.h"
#include "..\math\tweening.h"

namespace ds {

	enum ActionType {
		AT_SCALE,

		AT_ALPHA_FADE_TO,
		AT_COLOR_FADE_TO,
		AT_MOVE_TO,
		AT_MOVE_BY,
		AT_FOLLOW_PATH,
		AT_FOLLOW_CURVE,
		AT_FOLLOW_TARGET,
		AT_REMOVE_AFTER,
		AT_WAIT,
		AT_MOVE_WITH,
		AT_ROTATE,
		AT_ROTATE_BY,
		AT_FOLLOW_STRAIGHT_PATH,
		AT_KILL,
		AT_SCALE_BY_PATH,
		AT_COLOR_FLASH
	};

	struct ActionEvent {

		ID id;
		ActionType type;
		int spriteType;
	};

	struct ActionEventBuffer {

		Array<ActionEvent> events;

		void reset() {
			events.clear();
		}

		void add(ID id, ActionType type, int spriteType) {
			ActionEvent e;
			e.id = id;
			e.type = type;
			e.spriteType = spriteType;
			events.push_back(e);
		}
	};

	class AbstractAction;

	class World {

	public:
		World(int channels);
		~World();
		ID create();
		v4* getChannel(int index) const;
		uint32_t size() const;
		void scale(ID id, int channel, const v4& start, const v4& end, float ttl, int mode = 0, const tweening::TweeningType& tweeningType = &tweening::linear);
		void tick(float dt);
		void remove(ID id);
	private:
		int _numChannels;
		MultiplexArray* _array;
		AbstractAction* _actions[32];
		ActionEventBuffer _buffer;
	};

}
