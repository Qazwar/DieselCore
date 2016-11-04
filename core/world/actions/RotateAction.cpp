#include "RotateAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\GameMath.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	RotateAction::RotateAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "rotate") {
		int sizes[] = { sizeof(ID), sizeof(v3), sizeof(float), sizeof(float)};
		_buffer.init(sizes, 4);
	}

	

	void RotateAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_velocities = (v3*)_buffer.get_ptr(1);
			_timers = (float*)_buffer.get_ptr(2);
			_ttl = (float*)_buffer.get_ptr(3);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateAction::attach(ID id,const v3& velocity,float ttl) {
		int idx = create(id);
		_ids[idx] = id;
		_velocities[idx] = velocity;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateAction::update(float dt,ActionEventBuffer& buffer) {	
		if (_buffer.size > 0) {
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				v3 r = _array->get<v3>(_ids[i], WEC_ROTATION);
				r += _velocities[i] * dt;
				_array->set<v3>(_ids[i], WEC_ROTATION, r);
				if (_ttl[i] >= 0.0f) {
					_timers[i] += dt;
					if (_timers[i] >= _ttl[i]) {
						buffer.add(_ids[i], AT_ROTATE, _array->get<int>(_ids[i], WEC_TYPE));
						removeByIndex(i);
					}
				}
			}
		}
	}

	void RotateAction::saveReport(const ReportWriter & writer) {
	}

}