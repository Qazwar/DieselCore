#include "LookAtAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\math.h"
#include "..\..\base\Assert.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	LookAtAction::LookAtAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "look_at") {
		int sizes[] = { sizeof(ID), sizeof(ID), sizeof(float), sizeof(float) };
		_buffer.init(sizes, 4);
	}

	void LookAtAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_targets = (ID*)_buffer.get_ptr(1);
			_timers = (float*)_buffer.get_ptr(2);
			_ttl = (float*)_buffer.get_ptr(3);
		}
	}

	void LookAtAction::attach(ID id, ActionSettings* settings) {
		LookAtSettings* s = (LookAtSettings*)settings;
		ID target = INVALID_ID;
		int* indices = _array->_sparse;
		for (int i = 0; i < _array->capacity; ++i) {
			if (indices[i] != -1) {
				StaticHash current = _array->get<StaticHash>(i, WEC_HASH);
				if (s->target == current) {
					target = i;
				}
			}
		}
		XASSERT(target != INVALID_ID, "Cannot find target");
		if (target != INVALID_ID) {
			attach(id, target, s->ttl);
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void LookAtAction::attach(ID id, ID target, float ttl) {
		if (ttl == 0.0f) {
			v3 p = _array->get<v3>(id, WEC_POSITION);
			v3 t = _array->get<v3>(target, WEC_POSITION);
			v3 r = _array->get<v3>(id, WEC_ROTATION);
			r.x = math::getAngle(p.xy(), t.xy());
			_array->set<v3>(id, WEC_ROTATION, r);
		}
		else {
			int idx = create(id);
			_ids[idx] = id;
			_targets[idx] = target;
			_timers[idx] = 0.0f;
			_ttl[idx] = ttl;
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void LookAtAction::update(float dt,ActionEventBuffer& buffer) {
		if (_buffer.size > 0) {
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				v3 p = _array->get<v3>(_ids[i], WEC_POSITION);
				v3 t = _array->get<v3>(_targets[i],WEC_POSITION);
				v3 r = _array->get<v3>(_ids[i], WEC_ROTATION);
				r.x = math::getAngle(p.xy(), t.xy());
				_array->set<v3>(_ids[i], WEC_ROTATION, r);
				if (_ttl[i] > 0.0f) {
					_timers[i] += dt;
					if (_timers[i] >= _ttl[i]) {
						int t = _array->get<int>(_ids[i], WEC_TYPE);
						buffer.add(_ids[i], AT_LOOK_AT, t);
						removeByIndex(i);
					}
				}
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void LookAtAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.addSubHeader("LookAtAction");
			const char* HEADERS[] = { "Index", "ID", "Target", "Timer", "TTL"};
			writer.startTable(HEADERS, 5);
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(i);
				writer.addCell(_ids[i]);
				writer.addCell(_targets[i]);
				writer.addCell(_timers[i]);
				writer.addCell(_ttl[i]);
				writer.endRow();
			}
			writer.endTable();
		}
	}

}