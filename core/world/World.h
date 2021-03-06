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
#include "WorldEntityTemplates.h"
#include "ActionManager.h"
#include "Behaviors.h"

namespace ds {

	enum BounceDirection {
		BD_X,
		BD_Y,
		BD_BOTH,
		BD_EOL
	};

	enum WorldEntityChannel {
		WEC_POSITION,WEC_SCALE,WEC_ROTATION,WEC_TEXTURE,WEC_COLOR,WEC_TIMER,WEC_TYPE,WEC_FORCE,WEC_NAME,WEC_HASH
	};
	
	class AbstractAction;
	class CollisionAction;
	struct ActionSettings;

	class World {

	public:
		World();
		~World();
		void setWorldDimension(const v2& dim);
		void setBoundingRect(const Rect& r);
		void useTemplates(WorldEntityTemplates* templates);
		ID create();
		ID create(const v2& pos,StaticHash entityHash);
		ID create(const v2& pos, const Texture& texture, int type, float rotation = 0.0f, const v2& scale = v2(1,1), const Color& color = Color::WHITE);
		uint32_t size() const;
		bool contains(ID id) const;
		void attachName(ID id, const char* name);
		ID findByName(StaticHash hash);

		void ignoreCollisions(int firstType, int secondType);
		void attachCollider(ID id, ShapeType type, const v2& extent);
		void attachCollider(ID id, ShapeType type);
		bool hasCollisions() const;
		const Collision& getCollision(int idx) const;
		uint32_t numCollisions() const;		
		void moveBy(ID id, const v2& velocity, float ttl = -1.0f, bool bounce = true);
		void moveBy(ID id, const v3& velocity, float ttl = -1.0f, bool bounce = true);
		void moveTo(ID id, const v3& start, const v3& end, float ttl, const tweening::TweeningType& tweeningType = &tweening::linear);
		void scaleByPath(ID id, V3Path* path, float ttl);
		void scale(ID id, const v3& start, const v3& end, float ttl, int mode = 0, const tweening::TweeningType& tweeningType = &tweening::linear);
		void scaleAxes(ID id, int axes, float start, float end, float ttl, int mode = 0, const tweening::TweeningType& tweeningType = &tweening::linear);
		void removeAfter(ID sid, float ttl);
		void rotate(ID id, const v3& velocity, float ttl);
		void rotateBy(ID id, float angle, float ttl);
		void rotateTo(ID id, ID target, float angleVelocity);
		void lookAt(ID id, ID target, float ttl = -1.0f);
		void seek(ID id, ID target, float velocity);
		void separate(ID id, int type, float minDistance, float relaxation);
		void alphaFadeTo(ID id, float start, float end, float ttl);
		void stopAction(ID id, ActionType type);
		bool isActive(ID id, ActionType type);
		void flashColor(ID id, const Color& startColor, const Color& endColor, float ttl, int mode = 0, const tweening::TweeningType& tweeningType = &tweening::linear);

		void setPosition(ID id, const v2& pos);
		void setPosition(ID id, const v3& pos);
		const v3& getPosition(ID id) const;
		void setRotation(ID id, const v3& rotation);
		void setRotation(ID id, float rotation);
		const v3& getRotation(ID id) const;
		void setColor(ID id, const Color& color);
		const v3& getScale(ID id) const;
		void setScale(ID id, const v3& s);
		int getType(ID id) const;
		void setTexture(ID id, const Texture& texture);
		void tick(float dt);
		void remove(ID id);
		void removeByType(int type);
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
		template<class T>
		T* addCustomAction(const char* name) {
			T* t = new T(_data, _boundingRect, name);
			_customActions.push_back(t);
			return t;
		}
		void generateJSON(std::string& resp);

		ID createBehavior(const char* name);
		ID createBehavior(const char* name, ds::ActionDefinition* definitions, int num);
		void addSettings(ID behaviorID, ActionSettings* settings);
		void addSettings(SettingsDefinition* definitions, int num);
		void startBehavior(const StaticHash& hash, ID id);
		void connectBehaviors(ID first, const ActionType& type, ID second, int objectType);
		void connectBehaviors(ConnectionDefinition* definitions, int num, int objectType);
		void connectBehaviors(StaticHash first, const ActionType& type, StaticHash second, int objectType);
	private:
		int _numChannels;
		AdditionalData _additionalData;
		ChannelArray* _data;
		ActionManager* _actionManager;
		Array<AbstractAction*> _customActions;
		ActionEventBuffer _buffer;
		Rect _boundingRect;
		WorldEntityTemplates* _templates;
		Behaviors* _behaviors;
	};

}
