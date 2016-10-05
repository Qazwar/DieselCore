#include "MoveByAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\math.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	MoveByAction::MoveByAction(ChannelArray* array) : AbstractAction(array, "move_by") {
		int sizes[] = { sizeof(ID), sizeof(v3), sizeof(bool) };
		_buffer.init(sizes, 3);
	}

	void MoveByAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_velocities = (v3*)_buffer.get_ptr(1);
			_bounce = (bool*)_buffer.get_ptr(2);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::attach(ID id,const v3& velocity,bool bounce) {		
		int idx = create(id);
		_ids[idx] = id;
		_velocities[idx] = velocity;
		_bounce[idx] = bounce;
		rotateTo(idx);
	}

	void MoveByAction::rotateTo(int index) {
		float angle = math::calculateRotation(_velocities[index].xy());
		_array->set<v3>(_ids[index],WEC_ROTATION,v3(angle));
	}

	bool MoveByAction::isOutOfBounds(const v3& pos, const v3& v) {
		if (v.x > 0.0f && pos.x > m_BoundingRect.right) {
			return true;
		}
		if (v.x < 0.0f && pos.x < m_BoundingRect.left) {
			return true;
		}
		if (v.y > 0.0f && pos.y > m_BoundingRect.bottom) {
			return true;
		}
		if (v.y < 0.0f && pos.y < m_BoundingRect.top) {
			return true;
		}
		return false;
	}
	
	void MoveByAction::bounce(ID sid, BounceDirection direction,float dt) {
		for (int i = 0; i < _buffer.size; ++i) {
			if (_ids[i] == sid) {
				if (direction == BD_Y || direction == BD_BOTH) {
					_velocities[i].y *= -1.0f;
				}
				if (direction == BD_X || direction == BD_BOTH) {
					_velocities[i].x *= -1.0f;
				}
				float angle = math::calculateRotation(_velocities[i].xy());
				_array->set<v3>(sid,WEC_ROTATION,v3(angle));
				v3 p = _array->get<v3>(sid,WEC_POSITION);
				p += _velocities[i] * dt;
				_array->set<v3>(sid, WEC_POSITION, p);
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::update(float dt,ActionEventBuffer& buffer) {	
		if (_buffer.size > 0) {
			for (int i = 0; i < _buffer.size; ++i) {
				v3 p = _array->get<v3>(_ids[i],WEC_POSITION);
				p += _velocities[i] * dt;
				if (isOutOfBounds(p, _velocities[i])) {
					if (_bounce[i]) {
						if (p.y > m_BoundingRect.bottom || p.y < m_BoundingRect.top) {
							_velocities[i].y *= -1.0f;
						}
						if (p.x < m_BoundingRect.left || p.x > m_BoundingRect.right ) {
							_velocities[i].x *= -1.0f;
						}
						buffer.add(_ids[i], AT_BOUNCE, -1, &_velocities[i], sizeof(v3));
						rotateTo(i);
						p += _velocities[i] * dt * 1.5f;
					}
					else {
						int t = _array->get<int>(_ids[i], WEC_TYPE);
						buffer.add(_ids[i], AT_MOVE_BY, t);
					}
				}
				_array->set<v3>(_ids[i],WEC_POSITION, p);
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::debug() {
		LOG << "------- MoveByAction -------";
		for (int i = 0; i < _buffer.size; ++i) {
			LOG << i << " id: " << _ids[i] << " velocity: " << DBG_V2(_velocities[i]);
		}		
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::debug(ID sid) {
		int i = find(sid);
		if (i != -1) {
			LOG << "MoveByAction - id: " << _ids[i] << " velocity: " << DBG_V2(_velocities[i]);
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.addSubHeader("MoveByAction");
			const char* HEADERS[] = { "Index", "ID", "Velocity", "Bounce"};
			writer.startTable(HEADERS, 4);
			for (int i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(i);
				writer.addCell(_ids[i]);
				writer.addCell(_velocities[i]);
				writer.addCell(_bounce[i]);
				writer.endRow();
			}
			writer.endTable();
		}
	}

}