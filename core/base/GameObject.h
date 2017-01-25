#pragma once
#include "StateObject.h"
#include "..\string\StaticHash.h"

namespace ds {

	class GameObject : public StateObject {

	public:
		GameObject(const char* name) : StateObject(), _name(name) {
			_hash = StaticHash(name);
		}
		virtual ~GameObject() {}
		virtual void tick(float dt) {}
		virtual void render() {}
		const char* getName() const {
			return _name;
		}
		const StaticHash& getHash() const {
			return _hash;
		}
	private:
		StaticHash _hash;
		const char* _name;
	};

	namespace game {

		void init();

		void add_game_object(GameObject* object);

		GameObject* get_game_object(const StaticHash& hash);

		void update_game_objects(float dt);

		void render_game_objects();

		void activate_game_object(const StaticHash& hash);

		void deactivate_game_object(const StaticHash& hash);

		void shutdown();

	}

}