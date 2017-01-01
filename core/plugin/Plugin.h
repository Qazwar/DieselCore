#pragma once
#include <Windows.h>
#include "..\string\StaticHash.h"
#include "..\base\InputStates.h"

namespace ds {

	class Plugin {

	public:
		Plugin(const char* name, bool active = true) : _name(name) , _active(active) {
			_hash = SID(name);
		}
		virtual ~Plugin() {}
		virtual void tick(float dt) {}
		virtual void preRender() {}
		virtual void postRender() {}
		virtual void activate() {
			_active = true;
		}
		virtual void deactivate() {
			_active = false;
		}
		virtual void onButtonDown(int button, int x, int y) {}
		virtual void onButtonUp(int button, int x, int y) {}
		virtual void onChar(int ascii) {}
		const char* getName() const {
			return _name;
		}

		bool isActive() const {
			return _active;
		}
		const StaticHash& getHash() const {
			return _hash;
		}
	protected:
		bool _active;
	private:
		StaticHash _hash;
		const char* _name;		
	};

	namespace plugins {

		void init();

		void shutdown();

		void tick(float dt);

		void preRender();

		void postRender();

		void handleInput(const KeyStates& keyStates,const ButtonState& buttonState);

		void add(Plugin* plugin);

		void activate(const StaticHash& hash);

		void deactivate(const StaticHash& hash);

		Plugin* get(const StaticHash& hash);

	}
}
