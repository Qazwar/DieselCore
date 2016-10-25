#include "AlphaFadeToAction.h"
#include "..\..\math\math.h"
#include "..\..\log\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	AlphaFadeToAction::AlphaFadeToAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "alpha_fade") {
		int sizes[] = { sizeof(ID), sizeof(float), sizeof(float),sizeof(float),sizeof(float)};
		_buffer.init(sizes, 5);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	AlphaFadeToAction::~AlphaFadeToAction() {}

	void AlphaFadeToAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_startAlphas = (float*)_buffer.get_ptr(1);
			_endAlphas = (float*)_buffer.get_ptr(2);
			_timers = (float*)_buffer.get_ptr(3);
			_ttl = (float*)_buffer.get_ptr(4);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void AlphaFadeToAction::attach(ID id,float startAlpha,float endAlpha,float ttl) {
		int idx = create(id);
		_ids[idx] = id;
		_startAlphas[idx] = startAlpha;
		_endAlphas[idx] = endAlpha;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void AlphaFadeToAction::update(float dt,ActionEventBuffer& buffer) {	
		if ( _buffer.size > 0 ) {				
			// move
			for ( int i = 0; i < _buffer.size; ++i ) {
				float norm = _timers[i] / _ttl[i];
				Color c = _array->get<Color>(_ids[i], WEC_COLOR);
				c.a = _startAlphas[i] * (1.0f - norm) + _endAlphas[i] * norm;
				_array->set(_ids[i],WEC_COLOR, c);
				_timers[i] += dt;
				if ( _timers[i] >= _ttl[i] ) {
					c.a = _endAlphas[i];
					_array->set(_ids[i], WEC_COLOR, c);
					removeByIndex(i);
				}
			}
		}
	}

	void AlphaFadeToAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.startBox("AlphaFadeToAction");
			const char* OVERVIEW_HEADERS[] = { "ID", "Start", "End", "TTL", "Timer" };
			writer.startTable(OVERVIEW_HEADERS, 5);
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(_ids[i]);
				writer.addCell(_startAlphas[i]);
				writer.addCell(_endAlphas[i]);
				writer.addCell(_ttl[i]);
				writer.addCell(_timers[i]);
				writer.endRow();
			}
			writer.endTable();
			writer.endBox();
		}
	}
	

}