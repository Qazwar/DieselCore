#include "RemoveAfterAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\GameMath.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	RemoveAfterAction::RemoveAfterAction(ChannelArray* array) : AbstractAction(array, "remove_after") {
		int sizes[] = { sizeof(ID), sizeof(float), sizeof(float) };
		_buffer.init(sizes, 3);
	}

	void RemoveAfterAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_timers = (float*)_buffer.get_ptr(1);
			_ttl = (float*)_buffer.get_ptr(2);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RemoveAfterAction::attach(ID id, float ttl) {
		int idx = create(id);
		_ids[idx] = id;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RemoveAfterAction::update(float dt, ActionEventBuffer& buffer) {
		if ( _buffer.size > 0 ) {				
			// move
			for (int i = 0; i < _buffer.size; ++i) {
				_timers[i] += dt;
				if ( _timers[i] >= _ttl[i] ) {
					int t = _array->get<int>(_ids[i], WEC_TYPE);
					buffer.add(_ids[i], AT_KILL, t);
					removeByIndex(i);
				}
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RemoveAfterAction::debug() {
		LOG << "------- MoveToAction -------";
		for (int i = 0; i < _buffer.size; ++i) {
			LOG << i << " id: " << _ids[i] << " ttl: " << _ttl[i] << " timer: " << _timers[i];
		}		
	}

	void RemoveAfterAction::debug(ID sid) {
		int i = find(sid);
		if (i != -1) {
			LOG << "> move_to : id: " << _ids[i] << " ttl: " << _ttl[i] << " timer: " << _timers[i];
		}
		else {
			LOGE << "Object " << sid << " not found in action";
		}
	}

}