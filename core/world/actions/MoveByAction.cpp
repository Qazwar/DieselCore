#include "MoveByAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\math.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	MoveByAction::MoveByAction(ChannelArray* array) : AbstractAction(array, "move_by") {
		int sizes[] = { sizeof(ID), sizeof(v3), sizeof(float), sizeof(float), sizeof(bool) };
		_buffer.init(sizes, 5);
	}

	void MoveByAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_velocities = (v3*)_buffer.get_ptr(1);
			_timers = (float*)_buffer.get_ptr(2);
			_ttl = (float*)_buffer.get_ptr(3);
			_bounce = (bool*)_buffer.get_ptr(4);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::attach(ID id,const v3& velocity,float ttl, bool bounce) {		
		int idx = create(id);
		_ids[idx] = id;
		_velocities[idx] = velocity;
		_bounce[idx] = bounce;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
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
				v3 p = _array->get<v3>(sid,WEC_FORCE);
				p += _velocities[i] * dt;
				_array->set<v3>(sid, WEC_FORCE, p);
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::update(float dt,ActionEventBuffer& buffer) {	
		if (_buffer.size > 0) {
			for (int i = 0; i < _buffer.size; ++i) {
				v3 p = _array->get<v3>(_ids[i],WEC_FORCE);
				p += _velocities[i] * dt;
				v3 pos = _array->get<v3>(_ids[i], WEC_POSITION);
				pos += p;
				if (isOutOfBounds(pos, _velocities[i])) {
					if (_bounce[i]) {
						if (pos.y > m_BoundingRect.bottom || pos.y < m_BoundingRect.top) {
							_velocities[i].y *= -1.0f;
						}
						if (pos.x < m_BoundingRect.left || pos.x > m_BoundingRect.right) {
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
				if (_ttl[i] > 0.0f) {
					_timers[i] += dt;
					if (_timers[i] >= _ttl[i]) {
						int t = _array->get<int>(_ids[i], WEC_TYPE);
						buffer.add(_ids[i], AT_MOVE_BY, t);
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
	void MoveByAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.addSubHeader("MoveByAction");
			const char* HEADERS[] = { "Index", "ID", "Velocity", "Bounce"};
			writer.startTable(HEADERS, 4);
			for (uint32_t i = 0; i < _buffer.size; ++i) {
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