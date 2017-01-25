#include "GameObject.h"
#include "..\lib\collection_types.h"
#include "Assert.h"

namespace ds {

	namespace game {

		struct GameObjectContext {
			Array<GameObject*> objects;
		};

		static GameObjectContext* _goCtx = 0;

		void init() {
			_goCtx = new GameObjectContext;
		}

		static GameObject* findByHash(const StaticHash& hash) {
			for (uint32_t i = 0; i < _goCtx->objects.size(); ++i) {
				GameObject* o = _goCtx->objects[i];
				if (o->getHash() == hash) {
					return o;
				}
			}
			return 0;
		}

		void add_game_object(GameObject* object) {
			XASSERT(_goCtx != 0, "No GameObjectContext");
			_goCtx->objects.push_back(object);
		}

		GameObject* get_game_object(const StaticHash& hash) {
			XASSERT(_goCtx != 0, "No GameObjectContext");
			return findByHash(hash);
		}

		void update_game_objects(float dt) {
			XASSERT(_goCtx != 0, "No GameObjectContext");
			for (uint32_t i = 0; i < _goCtx->objects.size(); ++i) {
				GameObject* o = _goCtx->objects[i];
				if (o->isActive()) {
					o->tick(dt);
				}
			}
		}

		void render_game_objects() {
			XASSERT(_goCtx != 0, "No GameObjectContext");
			for (uint32_t i = 0; i < _goCtx->objects.size(); ++i) {
				GameObject* o = _goCtx->objects[i];
				if (o->isActive()) {
					o->render();
				}
			}
		}

		void activate_game_object(const StaticHash& hash) {
			XASSERT(_goCtx != 0, "No GameObjectContext");
			GameObject* go = findByHash(hash);
			if (go != 0) {
				go->activate();
			}
		}

		void deactivate_game_object(const StaticHash& hash) {
			XASSERT(_goCtx != 0, "No GameObjectContext");
			GameObject* go = findByHash(hash);
			if (go != 0) {
				go->deactivate();
			}
		}

		void shutdown() {
			if (_goCtx != 0) {
				_goCtx->objects.destroy_all();
				delete _goCtx;
			}
		}

	}

}