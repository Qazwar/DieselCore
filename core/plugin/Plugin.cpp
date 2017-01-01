#include "Plugin.h"
#include "..\lib\collection_types.h"
#include "..\profiler\Profiler.h"

namespace ds {

	namespace plugins {

		struct PluginContext {
			Array<Plugin*> plugins;
		};

		static PluginContext* _pluginCtx = nullptr;

		void init() {
			_pluginCtx = new PluginContext;
		}

		void shutdown() {
			if (_pluginCtx != nullptr) {
				_pluginCtx->plugins.destroy_all();
				delete _pluginCtx;
			}
		}

		void tick(float dt) {
			ZoneTracker zt("UPDATE::Plugins");
			uint32_t sz = _pluginCtx->plugins.size();
			for (uint32_t i = 0; i < sz; ++i) {
				if (_pluginCtx->plugins[i]->isActive()) {
					_pluginCtx->plugins[i]->tick(dt);
				}
			}
		}

		void preRender() {
			ZoneTracker("Render::Plugins-Pre");
			uint32_t sz = _pluginCtx->plugins.size();
			for (uint32_t i = 0; i < sz; ++i) {
				if (_pluginCtx->plugins[i]->isActive()) {
					_pluginCtx->plugins[i]->preRender();
				}
			}
		}

		void postRender() {
			ZoneTracker("Render::Plugins-Post");
			uint32_t sz = _pluginCtx->plugins.size();
			for (uint32_t i = 0; i < sz; ++i) {
				if (_pluginCtx->plugins[i]->isActive()) {
					_pluginCtx->plugins[i]->postRender();
				}
			}
		}

		void handleInput(const KeyStates& keyStates, const ButtonState& buttonState) {
			// FIXME: should we continue with processing or stop if any plugin has processed any input????
			ZoneTracker z("INPUT:Plugin");
			uint32_t sz = _pluginCtx->plugins.size();
			for (uint32_t i = 0; i < sz; ++i) {
				if (_pluginCtx->plugins[i]->isActive()) {
					if (keyStates.onChar) {
						_pluginCtx->plugins[i]->onChar(keyStates.ascii);
					}
					if (!buttonState.processed) {
						if (buttonState.down) {
							_pluginCtx->plugins[i]->onButtonDown(buttonState.button, buttonState.x, buttonState.y);
						}
						else {
							_pluginCtx->plugins[i]->onButtonUp(buttonState.button, buttonState.x, buttonState.y);
						}
					}
				}
			}
		}

		void add(Plugin* plugin) {
			_pluginCtx->plugins.push_back(plugin);
		}

		void activate(const StaticHash& hash) {
			Plugin* p = plugins::get(hash);
			if (p != nullptr) {
				p->deactivate();
			}
		}

		void deactivate(const StaticHash& hash) {
			Plugin* p = plugins::get(hash);
			if (p != nullptr) {
				p->deactivate();
			}
		}

		Plugin* get(const StaticHash& hash) {
			uint32_t sz = _pluginCtx->plugins.size();
			for (uint32_t i = 0; i < sz; ++i) {
				if (_pluginCtx->plugins[i]->getHash() == hash) {
					return _pluginCtx->plugins[i];
				}
			}
			return nullptr;
		}
	}
}