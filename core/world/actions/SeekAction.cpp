#include "SeekAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\math.h"
#include "..\..\base\Assert.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	SeekAction::SeekAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "seek") {
		int sizes[] = { sizeof(ID), sizeof(ID), sizeof(float) };
		_buffer.init(sizes, 3);
	}

	void SeekAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_targets = (ID*)_buffer.get_ptr(1);
			_velocities = (float*)_buffer.get_ptr(2);
		}
	}

	void SeekAction::attach(ID id, ActionSettings* settings) {
		SeekSettings* s = (SeekSettings*)settings;
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
			attach(id, target, s->velocity);
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void SeekAction::attach(ID id, ID target, float velocity) {
		int idx = create(id);
		_ids[idx] = id;
		_targets[idx] = target;
		_velocities[idx] = velocity;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void SeekAction::update(float dt,ActionEventBuffer& buffer) {
		if (_buffer.size > 0) {
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				v3 p = _array->get<v3>(_ids[i], WEC_POSITION);
				v3 t = _array->get<v3>(_targets[i],WEC_POSITION);
				v3 f = _array->get<v3>(_ids[i], WEC_FORCE);

				v3 diff = t - p;
				v3 n = normalize(diff);
				v3 desired = n * _velocities[i];
				f += desired * dt;

				v3 r = _array->get<v3>(_ids[i], WEC_ROTATION);
				r.x = math::getAngle(p.xy(), t.xy());
				_array->set<v3>(_ids[i], WEC_ROTATION, r);
				
				_array->set<v3>(_ids[i], WEC_FORCE, f);
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void SeekAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.addSubHeader("SeekAction");
			const char* HEADERS[] = { "Index", "ID", "Target", "Velocity"};
			writer.startTable(HEADERS, 4);
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(i);
				writer.addCell(_ids[i]);
				writer.addCell(_targets[i]);
				writer.addCell(_velocities[i]);
				writer.endRow();
			}
			writer.endTable();
		}
	}

}