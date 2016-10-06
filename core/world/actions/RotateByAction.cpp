#include "RotateByAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\GameMath.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	RotateByAction::RotateByAction(ChannelArray* array) : AbstractAction(array, "rotate_by") {
		int sizes[] = { sizeof(ID), sizeof(float), sizeof(float), sizeof(float)};
		_buffer.init(sizes, 4);
	}

	

	void RotateByAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_angles = (float*)_buffer.get_ptr(1);
			_timers = (float*)_buffer.get_ptr(2);
			_ttl = (float*)_buffer.get_ptr(3);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateByAction::attach(ID id,float angle,float ttl) {
		int idx = create(id);
		_ids[idx] = id;
		_angles[idx] = angle;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateByAction::update(float dt,ActionEventBuffer& buffer) {	
		if (_buffer.size > 0) {
			// move
			for (int i = 0; i < _buffer.size; ++i) {
				v3 r = _array->get<v3>(_ids[i], WEC_ROTATION);
				r.x += _angles[i] * dt;
				r.y += _angles[i] * dt;
				r.z += _angles[i] * dt;
				_array->set<v3>(_ids[i], WEC_ROTATION, r);
				_timers[i] += dt;
				if ( _timers[i] >= _ttl[i] ) {
					buffer.add(_ids[i], AT_ROTATE_BY, _array->get<int>(_ids[i],WEC_TYPE));
					removeByIndex(i);
				}
			}
		}
	}

	void RotateByAction::saveReport(const ReportWriter & writer) {
	}

}