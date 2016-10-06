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
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				_timers[i] += dt;
				if ( _timers[i] >= _ttl[i] ) {
					int t = _array->get<int>(_ids[i], WEC_TYPE);
					buffer.add(_ids[i], AT_KILL, t);
				}
			}
		}
	}

	void RemoveAfterAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.startBox("RemoveAfterAction");
			const char* OVERVIEW_HEADERS[] = { "ID", "TTL", "Timer" };
			writer.startTable(OVERVIEW_HEADERS, 3);
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(_ids[i]);
				writer.addCell(_ttl[i]);
				writer.addCell(_timers[i]);
				writer.endRow();
			}
			writer.endTable();
			writer.endBox();
		}
	}

}