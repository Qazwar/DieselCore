#pragma once
#include <stdint.h>
#include "..\lib\collection_types.h"
#include "EventStream.h"

namespace ds {

	struct Shortcut {
		const char* label;
		char key;
		uint32_t eventType;
	};

	class ShortcutsHandler {

	public:
		ShortcutsHandler();
		~ShortcutsHandler();
		void debug();
		void handleInput(char ascii);
		void add(const char* label, char key, uint32_t eventType);
		//void showDialog();
	private:
		Array<Shortcut> _shortcuts;
		Array<const char*> _model;
	};

}

