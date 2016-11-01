#pragma once
#include "..\Common.h"
#include "..\lib\collection_types.h"

namespace ds {

	enum ActionType {
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
		AT_SCALE,
		AT_COLOR_FLASH,
		AT_ROTATE_TO,
		AT_COLLISION,
		AT_BOUNCE,
		AT_ROTATE_TO_TARGET,
		AT_LOOK_AT,
		AT_SEEK,
		AT_SEPARATE,
		AT_CUSTOM,
		AT_SCALE_AXES
	};

	struct ActionEvent {

		ID id;
		ActionType action;
		int type;
		int dataIndex;
	};

	struct ActionEventBuffer {

		Array<ActionEvent> events;
		CharBuffer data;

		void reset() {
			events.clear();
			data.size = 0;
		}


		void add(ID id, ActionType action, int type) {
			ActionEvent e;
			e.id = id;
			e.type = type;
			e.action = action;
			e.dataIndex = -1;
			events.push_back(e);
		}

		void add(ID id, ActionType action, int type,void *d,int size) {
			ActionEvent e;
			e.id = id;
			e.type = type;
			e.action = action;
			e.dataIndex = data.size;
			void* dest = data.alloc(size);
			memcpy(dest, d, size);
			events.push_back(e);
		}

		void* get(int dataIndex) const {
			return (void*)(data.data + dataIndex);
		}
	};

}