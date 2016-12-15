#include "WiggleAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\math.h"

namespace ds {

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	WiggleAction::WiggleAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "wiggle") {
		int sizes[] = { sizeof(ID), sizeof(float), sizeof(float), sizeof(float), sizeof(float)};
		_buffer.init(sizes, 5);
	}

	void WiggleAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_amplitudes = (float*)_buffer.get_ptr(1);
			_frequencies = (float*)_buffer.get_ptr(2);
			_timers = (float*)_buffer.get_ptr(3);
			_ttl = (float*)_buffer.get_ptr(4);
		}
	}

	void WiggleAction::attach(ID id, ActionSettings* settings) {
		WiggleSettings* s = (WiggleSettings*)settings;
		attach(id, s->amplitude, s->frequency, s->ttl);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void WiggleAction::attach(ID id, float amplitude, float frequency, float ttl) {
		int idx = create(id);
		_ids[idx] = id;
		_amplitudes[idx] = amplitude;
		_frequencies[idx] = frequency;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void WiggleAction::update(float dt, ActionEventBuffer& buffer) {
		if (_buffer.size > 0) {
			for (int i = 0; i < _buffer.size; ++i) {
				_timers[i] += dt;
				v3 p = _array->get<v3>(_ids[i],WEC_FORCE);
				v3 r = _array->get<v3>(_ids[i], WEC_ROTATION);
				float angle = r.x + DEGTORAD(90.0f);
				float x = angle + cos(_timers[i] * _frequencies[i]) * _amplitudes[i];
				float y = angle + sin(_timers[i] * _frequencies[i]) * _amplitudes[i];
				v3 add = v3(x, y, 0.0f);
				p += add * dt;
				if (_ttl[i] > 0.0f) {
					if (_timers[i] >= _ttl[i]) {
						int t = _array->get<int>(_ids[i], WEC_TYPE);
						buffer.add(_ids[i], AT_WIGGLE, t);
						removeByIndex(i);
					}
				}
				_array->set<v3>(_ids[i],WEC_FORCE, p);
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void WiggleAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.addSubHeader("Wiggle");
			const char* HEADERS[] = { "Index", "ID", "Amplitude", "Frequency"};
			writer.startTable(HEADERS, 4);
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(i);
				writer.addCell(_ids[i]);
				writer.addCell(_amplitudes[i]);
				writer.addCell(_frequencies[i]);
				writer.endRow();
			}
			writer.endTable();
		}
	}

}