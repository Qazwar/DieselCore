#include "ScaleAxesAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\log\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ScaleAxesAction::ScaleAxesAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "scale_axes") {
		int sizes[] = { sizeof(ID), sizeof(int), sizeof(float), sizeof(float), sizeof(int), sizeof(float), sizeof(float), sizeof(tweening::TweeningType), sizeof(int) };
		_buffer.init(sizes, 9);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ScaleAxesAction::~ScaleAxesAction() {}

	void ScaleAxesAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_channels = (int*)_buffer.get_ptr(1);
			_startScale = (float*)_buffer.get_ptr(2);
			_endScale = (float*)_buffer.get_ptr(3);
			_axes = (int*)_buffer.get_ptr(4);
			_timers = (float*)_buffer.get_ptr(5);
			_ttl = (float*)_buffer.get_ptr(6);
			_tweeningTypes = (tweening::TweeningType*)_buffer.get_ptr(7);
			_modes = (int*)_buffer.get_ptr(8);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ScaleAxesAction::attach(ID id, int axes, float startScale,float endScale,float ttl,int mode,const tweening::TweeningType& tweeningType) {
		int idx = create(id);
		_ids[idx] = id;
		_axes[idx] = axes;
		_startScale[idx] = startScale;
		_endScale[idx] = endScale;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
		_tweeningTypes[idx] = tweeningType;
		_modes[idx] = mode;
		_array->set(id, WEC_SCALE, startScale);
		if ( mode > 0 ) {
			--_modes[idx];
		}

	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ScaleAxesAction::update(float dt,ActionEventBuffer& buffer) {
		if (_buffer.size > 0) {
			// move
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				_timers[i] += dt;
				v3 r = _array->get<v3>(_ids[i], WEC_SCALE);
				r.data[_axes[i]] = tweening::interpolate(_tweeningTypes[i], _startScale[i], _endScale[i], _timers[i], _ttl[i]);
				_array->set(_ids[i], WEC_SCALE, r);				
				if ( _timers[i] >= _ttl[i] ) {
					if ( _modes[i] < 0 ) {
						_timers[i] = 0.0f;
					}
					else if ( _modes[i] == 0 ) {
						_array->set(_ids[i], WEC_SCALE, _endScale[i]);
						buffer.add(_ids[i], AT_SCALE_AXES, _array->get<int>(_ids[i],WEC_TYPE));
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

	void ScaleAxesAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.startBox("ScalingAction");
			const char* OVERVIEW_HEADERS[] = { "ID", "Axes", "Start", "End", "TTL", "Timer" };
			writer.startTable(OVERVIEW_HEADERS, 6);
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(_ids[i]);
				writer.addCell(_axes[i]);
				writer.addCell(_startScale[i]);
				writer.addCell(_endScale[i]);
				writer.addCell(_ttl[i]);
				writer.addCell(_timers[i]);
				writer.endRow();
			}
			writer.endTable();
			writer.endBox();
		}
	}

}