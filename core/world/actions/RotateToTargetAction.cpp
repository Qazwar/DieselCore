#include "RotateToTargetAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\GameMath.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	RotateToTargetAction::RotateToTargetAction(ChannelArray* array) : AbstractAction(array, "rotate_to_target") {
		int sizes[] = { sizeof(ID), sizeof(ID), sizeof(float)};
		_buffer.init(sizes, 3);
	}

	

	void RotateToTargetAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_targets = (ID*)_buffer.get_ptr(1);
			_angleVelocities = (float*)_buffer.get_ptr(2);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateToTargetAction::attach(ID id, ID target, float angleVelocity) {
		int idx = create(id);
		_ids[idx] = id;
		_targets[idx] = target;
		_angleVelocities[idx] = angleVelocity;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateToTargetAction::update(float dt, ActionEventBuffer& buffer) {
		if (_buffer.size > 0) {
			// move
			for (int i = 0; i < _buffer.size; ++i) {
				v3 current = _array->get<v3>(_ids[i], WEC_POSITION);
				v3 target = _array->get<v3>(_targets[i], WEC_POSITION);
				v3 diff = target - current;
				float angle = math::getAngle(V2_RIGHT, diff.xy());
				v3 r = _array->get<v3>(_ids[i], WEC_ROTATION);
				float delta = angle - r.x;
				if ( abs(delta) <= DEGTORAD(5.0f) ) {
					buffer.add(_ids[i], AT_ROTATE_TO_TARGET, _array->get<int>(_ids[i],WEC_TYPE));
					removeByIndex(i);
				}
				else {										
					if (delta < PI) {
						r.x += _angleVelocities[i] * dt;
					}
					else {
						r.x -= _angleVelocities[i] * dt;
					}
					if (r.x > TWO_PI) {
						r.x -= TWO_PI;
					}
					if (r.x < 0.0f) {
						r.x += TWO_PI;
					}
					_array->set<v3>(_ids[i], WEC_ROTATION, r);
				}
			}
		}
	}

	void RotateToTargetAction::saveReport(const ReportWriter & writer) {
	}

}