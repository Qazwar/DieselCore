#include "MoveToAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\math.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	MoveToAction::MoveToAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "move_to") {
		int sizes[] = { sizeof(ID), sizeof(v3), sizeof(v3), sizeof(tweening::TweeningType) , sizeof(float), sizeof(float) };
		_buffer.init(sizes, 6);
	}

	void MoveToAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_start = (v3*)_buffer.get_ptr(1);
			_end = (v3*)_buffer.get_ptr(2);
			_tweeningTypes = (tweening::TweeningType*)_buffer.get_ptr(3);
			_timers = (float*)_buffer.get_ptr(4);
			_ttl = (float*)_buffer.get_ptr(5);
		}
	}

	void MoveToAction::attach(ID id, ActionSettings* settings) {
		MoveToSettings* s = (MoveToSettings*)settings;		
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveToAction::attach(ID id, const v3& start, const v3& end, float ttl, const tweening::TweeningType& tweeningType) {
		int idx = create(id);
		_ids[idx] = id;
		_start[idx] = start;
		_end[idx] = end;
		_tweeningTypes[idx] = tweeningType;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
		//rotateTo(idx);
	}

	void MoveToAction::rotateTo(int index) {
		float angle = math::getAngle(_end[index].xy(), _start[index].xy());
		_array->set<v3>(_ids[index],WEC_ROTATION,v3(angle));
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveToAction::update(float dt,ActionEventBuffer& buffer) {
		if (_buffer.size > 0) {
			for (int i = 0; i < _buffer.size; ++i) {
				_array->set<v3>(_ids[i], WEC_POSITION, tweening::interpolate(_tweeningTypes[i], _start[i], _end[i], _timers[i], _ttl[i]));
				if (_ttl[i] > 0.0f) {
					_timers[i] += dt;
					if (_timers[i] >= _ttl[i]) {
						_array->set<v3>(_ids[i], WEC_POSITION, _end[i]);
						int t = _array->get<int>(_ids[i], WEC_TYPE);
						buffer.add(_ids[i], AT_MOVE_TO, t);
						removeByIndex(i);
					}
				}
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveToAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.addSubHeader("MoveToAction");
			const char* HEADERS[] = { "Index", "ID", "Start", "End"};
			writer.startTable(HEADERS, 4);
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(i);
				writer.addCell(_ids[i]);
				writer.addCell(_start[i]);
				writer.addCell(_end[i]);
				writer.endRow();
			}
			writer.endTable();
		}
	}

}