#include "ScalingAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\log\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ScalingAction::ScalingAction(MultiplexArray* array) : AbstractAction(array,"scale") {
		int sizes[] = { sizeof(ID), sizeof(int), sizeof(v4), sizeof(v4), sizeof(float), sizeof(float), sizeof(tweening::TweeningType), sizeof(int) };
		_buffer.init(sizes, 8);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ScalingAction::~ScalingAction() {}

	void ScalingAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_channels = (int*)_buffer.get_ptr(1);
			_startScale = (v4*)_buffer.get_ptr(2);
			_endScale = (v4*)_buffer.get_ptr(3);
			_timers = (float*)_buffer.get_ptr(4);
			_ttl = (float*)_buffer.get_ptr(5);
			_tweeningTypes = (tweening::TweeningType*)_buffer.get_ptr(6);
			_modes = (int*)_buffer.get_ptr(7);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ScalingAction::attach(ID id, int channel, const v4& startScale,const v4& endScale,float ttl,int mode,const tweening::TweeningType& tweeningType) {
		int idx = create(id);
		_ids[idx] = id;
		_channels[idx] = channel;
		_startScale[idx] = startScale;
		_endScale[idx] = endScale;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
		_tweeningTypes[idx] = tweeningType;
		_modes[idx] = mode;
		_array->set(id, channel, startScale);
		if ( mode > 0 ) {
			--_modes[idx];
		}

	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ScalingAction::update(float dt,ActionEventBuffer& buffer) {
		if (_buffer.size > 0) {
			// move
			for (int i = 0; i < _buffer.size; ++i) {
				_timers[i] += dt;
				v4 t = tweening::interpolate(_tweeningTypes[i], _startScale[i], _endScale[i], _timers[i], _ttl[i]);
				_array->set(_ids[i], _channels[i], t);				
				if ( _timers[i] >= _ttl[i] ) {
					if ( _modes[i] < 0 ) {
						_timers[i] = 0.0f;
					}
					else if ( _modes[i] == 0 ) {
						_array->set(_ids[i], _channels[i], _endScale[i]);
						//buffer.add(_ids[i], AT_SCALE, array.getType(_ids[i]));
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

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ScalingAction::debug() {
		if ( _buffer.size > 0 ) {
			LOG << "---------- ScalingAction ---------- ";
		}
		for ( int i = 0; i < _buffer.size; ++i ) {
			LOG << i << " : id: " << _ids[i] << " timer: " << _timers[i];
		}
		/*
		std::map<SID,int>::iterator it = m_Mapping.begin();
		while ( it != m_Mapping.end()) {
			LOG << it->first << " = " << it->second;
			++it;
		}
		*/
	}

}