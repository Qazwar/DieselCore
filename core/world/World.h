#pragma once
#include "..\lib\BlockArray.h"
#include "..\lib\collection_types.h"
#include "..\math\tweening.h"
#include "..\graphics\Texture.h"
#include "..\graphics\Color.h"
#include "ActionEventBuffer.h"
#include "..\math\FloatArray.h"

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
	
	class AbstractAction;

	class World {

	public:
		World();
		~World();
		ID create();
		ID create(const v2& pos, const Texture& texture, int type, float rotation = 0.0f, const v2& scale = v2(1,1), const Color& color = Color::WHITE);
		uint32_t size() const;

		void scaleByPath(ID id, V3Path* path, float ttl);
		void scale(ID id, const v3& start, const v3& end, float ttl, int mode = 0, const tweening::TweeningType& tweeningType = &tweening::linear);
		void removeAfter(ID sid, float ttl);

		void setPosition(ID id, const v2& pos);
		const v3& getPosition(ID id) const;
		void setRotation(ID id, const v3& rotation);
		void setRotation(ID id, float rotation);

		void tick(float dt);
		void remove(ID id);
		ChannelArray* getChannelArray() const {
			return _data;
		}
		int find_by_type(int type, ID* ids, int max) const;
	private:
		int _numChannels;
		ChannelArray* _data;
		AbstractAction* _actions[32];
		ActionEventBuffer _buffer;
	};

}
