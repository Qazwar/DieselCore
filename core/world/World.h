#pragma once
#include "..\lib\BlockArray.h"
#include "..\lib\collection_types.h"
#include "..\math\tweening.h"
#include "..\graphics\Texture.h"
#include "..\graphics\Color.h"
#include "ActionEventBuffer.h"
#include "..\math\FloatArray.h"
#include "..\io\ReportWriter.h"
#include "..\physics\ColliderArray.h"
#include "AdditionalData.h"

namespace ds {

	enum BounceDirection {
		BD_X,
		BD_Y,
		BD_BOTH,
		BD_EOL
	};

	enum ShapeType {
		PST_BOX,
		PST_CIRCLE,
		PST_QUAD,
		PST_SPHERE,
		PST_NONE
	};

	struct WorldEntity {

		v3 position;
		v3 scale;
		v3 rotation;
		Texture texture;
		Color color;
		float timer;
		int type;
		v3 force;
	};

	enum WorldEntityChannel {
		WEC_POSITION,WEC_SCALE,WEC_ROTATION,WEC_TEXTURE,WEC_COLOR,WEC_TIMER,WEC_TYPE,WEC_FORCE
	};
	
	class AbstractAction;
	class CollisionAction;

	class World {

	public:
		World();
		~World();
		ID create();
		ID create(const v2& pos, const Texture& texture, int type, float rotation = 0.0f, const v2& scale = v2(1,1), const Color& color = Color::WHITE);
		uint32_t size() const;
		bool contains(ID id) const;

		void ignoreCollisions(int firstType, int secondType);
		void attachCollider(ID id, ShapeType type, const v2& extent);
		bool hasCollisions() const;
		const Collision& getCollision(int idx) const;
		uint32_t numCollisions() const;

		void moveBy(ID id, const v2& velocity, float ttl = -1.0f, bool bounce = true);
		void moveBy(ID id, const v3& velocity, float ttl = -1.0f, bool bounce = true);
		void scaleByPath(ID id, V3Path* path, float ttl);
		void scale(ID id, const v3& start, const v3& end, float ttl, int mode = 0, const tweening::TweeningType& tweeningType = &tweening::linear);
		void removeAfter(ID sid, float ttl);
		void rotateBy(ID id, float angle, float ttl);
		void rotateTo(ID id, ID target, float angleVelocity);
		void lookAt(ID id, ID target, float ttl = -1.0f);
		void seek(ID id, ID target, float velocity);
		void separate(ID id, int type, float minDistance, float relaxation);
		void stopAction(ID id, ActionType type);

		void setPosition(ID id, const v2& pos);
		const v3& getPosition(ID id) const;
		void setRotation(ID id, const v3& rotation);
		void setRotation(ID id, float rotation);
		const v3& getRotation(ID id) const;
		void setColor(ID id, const Color& color);
		int getType(ID id) const;

		void tick(float dt);
		void remove(ID id);
		ChannelArray* getChannelArray() const {
			return _data;
		}
		int find_by_type(int type, ID* ids, int max) const;

		void saveReport(const ReportWriter& writer);

		void* attach_data(ID sid, int size, int identifier) {
			return _additionalData.attach(sid, size, identifier);
		}

		void* get_data(ID sid) {
			return _additionalData.get(sid);
		}

		bool hasEvents() const {
			return !_buffer.events.empty();
		}
		const ActionEvent& getEvent(int idx) const {
			return _buffer.events[idx];
		}
		void* getEventData(int idx) const {
			const ActionEvent& event = _buffer.events[idx];
			if (event.dataIndex != -1) {
				return _buffer.get(event.dataIndex);
			}
			return 0;
		}
		uint32_t numEvents() const {
			return _buffer.events.size();
		}
	private:
		int _numChannels;
		AdditionalData _additionalData;
		CollisionAction* _collisionAction;
		ChannelArray* _data;
		AbstractAction* _actions[32];
		ActionEventBuffer _buffer;
	};

}
