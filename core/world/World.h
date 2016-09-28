#pragma once
#include "..\lib\BlockArray.h"
#include "..\lib\collection_types.h"
#include "..\math\tweening.h"
#include "..\graphics\Texture.h"
#include "..\graphics\Color.h"
namespace ds {

	struct WorldEntity {

		v3 position;
		v3 scale;
		v3 rotation;
		Texture texture;
		Color color;
		float timer;
		int type;
		v3 previous;
		v3 extent;
		// Shapetype
	};

	enum WorldEntityChannel {
		WEC_POSITION,WEC_SCALE,WEC_ROTATION,WEC_TEXTURE,WEC_COLOR,WEC_TIMER,WEC_TYPE,WEC_PREVIOUS,WEC_EXTENT
	};

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
		ID create(const v2& pos, const Texture& texture, float rotation, const v2& scale, const Color& color = Color::WHITE);
		uint32_t size() const;
		void scale(ID id, const v3& start, const v3& end, float ttl, int mode = 0, const tweening::TweeningType& tweeningType = &tweening::linear);
		void tick(float dt);
		void remove(ID id);
		ChannelArray* getChannelArray() const {
			return _data;
		}
	private:
		int _numChannels;
		ChannelArray* _data;
		AbstractAction* _actions[32];
		ActionEventBuffer _buffer;
	};

}
