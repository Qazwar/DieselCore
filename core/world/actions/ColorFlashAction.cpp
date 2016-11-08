#include "ColorFlashAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\log\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ColorFlashAction::ColorFlashAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "flash_color") {
		int sizes[] = { sizeof(ID), sizeof(Color), sizeof(Color), sizeof(float), sizeof(float),sizeof(tweening::TweeningType),sizeof(int) };
		_buffer.init(sizes, 7);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ColorFlashAction::~ColorFlashAction() {}

	void ColorFlashAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_startColors = (Color*)_buffer.get_ptr(1);
			_endColors = (Color*)_buffer.get_ptr(2);
			_timers = (float*)_buffer.get_ptr(3);
			_ttl = (float*)_buffer.get_ptr(4);
			_tweeningTypes = (tweening::TweeningType*)_buffer.get_ptr(5);
			_modes = (int*)_buffer.get_ptr(6);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ColorFlashAction::attach(ID id, const Color& startColor, const Color& endColor, float ttl, int mode, const tweening::TweeningType& tweeningType) {
		int idx = create(id);
		_ids[idx] = id;
		_startColors[idx] = startColor;
		_endColors[idx] = endColor;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
		_tweeningTypes[idx] = tweeningType;
		_modes[idx] = mode;
		if ( mode > 0 ) {
			--_modes[idx];
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ColorFlashAction::update(float dt, ActionEventBuffer& buffer) {
		if ( _buffer.size > 0 ) {				
			for ( int i = 0; i < _buffer.size; ++i ) {
				_array->set<Color>(_ids[i], WEC_COLOR,tweening::interpolate(tweening::easeSinus, _startColors[i], _endColors[i], _timers[i], _ttl[i]));
				_timers[i] += dt;
				if ( _timers[i] >= _ttl[i] ) {
					if ( _modes[i] < 0 ) {
						_timers[i] = 0.0f;
					}
					else if ( _modes[i] == 0 ) {
						_array->set<Color>(_ids[i], WEC_COLOR, _startColors[i]);
						removeByIndex(i);
					}
					else {
						--_modes[i];
						_timers[i] = 0.0f;

					}
				}
			}
		}
	}

	void ColorFlashAction::saveReport(const ReportWriter & writer) {
	}

}