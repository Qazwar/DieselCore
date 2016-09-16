#include "ShortcutsHandler.h"
#include "..\log\Log.h"

namespace ds {

	ShortcutsHandler::ShortcutsHandler() {
	}


	ShortcutsHandler::~ShortcutsHandler() {
	}

	void ShortcutsHandler::debug() {
		LOG << "==> Shortcuts: ";
		for (int i = 0; i < _shortcuts.size(); ++i) {
			LOG << "key: " << _shortcuts[i].key << " -> " << _shortcuts[i].label << " = " << _shortcuts[i].eventType;
		}
	}

	void ShortcutsHandler::handleInput(char ascii) {
		for (int i = 0; i < _shortcuts.size(); ++i) {
			if (_shortcuts[i].key == ascii) {
				events::send(_shortcuts[i].eventType);
			}
		}
	}

	void ShortcutsHandler::add(const char* label, char key, uint32_t eventType) {
		Shortcut s;
		s.label = label;
		s.key = key;
		s.eventType = eventType;
		_shortcuts.push_back(s);
		_model.push_back(label);
	}

	/*
	void ShortcutsHandler::showDialog() {
		_position.y = renderer::getScreenHeight();
		gui::start(1, &_position, true);
		gui::begin();
		int selection = gui::ActionBar(_model);
		if (selection != -1) {
			_events->add(_shortcuts[selection].eventType);
		}
		gui::end();		
	}
	*/
}
