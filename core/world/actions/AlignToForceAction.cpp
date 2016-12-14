#include "AlignToForceAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\math.h"

namespace ds {

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	AlignToForceAction::AlignToForceAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "align_to_force") {
		int sizes[] = { sizeof(ID), sizeof(float), sizeof(float)};
		_buffer.init(sizes, 3);
	}

	void AlignToForceAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_timers = (float*)_buffer.get_ptr(1);
			_ttl = (float*)_buffer.get_ptr(2);
		}
	}

	void AlignToForceAction::attach(ID id, ActionSettings* settings) {
		AlignToForceActionSettings* s = (AlignToForceActionSettings*)settings;
		attach(id, s->ttl);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void AlignToForceAction::attach(ID id, float ttl) {
		int idx = create(id);
		_ids[idx] = id;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void AlignToForceAction::update(float dt, ActionEventBuffer& buffer) {
		if (_buffer.size > 0) {
			for (int i = 0; i < _buffer.size; ++i) {				
				v3 force = _array->get<v3>(_ids[i], WEC_FORCE);
				float angle = math::calculateRotation(force.xy());
				_array->set<v3>(_ids[i], WEC_ROTATION, v3(angle));
				if (_ttl[i] > 0.0f) {
					_timers[i] += dt;
					if (_timers[i] >= _ttl[i]) {
						int t = _array->get<int>(_ids[i], WEC_TYPE);
						buffer.add(_ids[i], AT_ALIGN_TO_FORCE, t);
						removeByIndex(i);
					}
				}
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void AlignToForceAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.addSubHeader("AlignToForce");
			const char* HEADERS[] = { "Index", "ID"};
			writer.startTable(HEADERS, 4);
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(i);
				writer.addCell(_ids[i]);
				writer.endRow();
			}
			writer.endTable();
		}
	}

}