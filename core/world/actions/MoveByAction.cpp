#include "MoveByAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\math.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	MoveByAction::MoveByAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "move_by") {
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

	int MoveByAction::isOutOfBounds(const v3& pos, const v3& v, const v2& dim) {
		int dir = 0;
		if (v.x > 0.0f && pos.x > (m_BoundingRect.right - dim.x)) {
			dir |= 1;
		}
		if (v.x < 0.0f && pos.x < (m_BoundingRect.left + dim.x)) {
			dir |= 2;
		}
		if (v.y > 0.0f && pos.y > (m_BoundingRect.bottom - dim.y)) {
			dir |= 4;
		}
		if (v.y < 0.0f && pos.y < (m_BoundingRect.top + dim.y)) {
			dir |= 8;
		}
		return dir;
	}
	
	void MoveByAction::bounce(ID sid, BounceDirection direction,float dt) {
		for (uint32_t i = 0; i < _buffer.size; ++i) {
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
				const Texture& t = _array->get<Texture>(_ids[i], WEC_TEXTURE);
				int d = isOutOfBounds(pos, _velocities[i], t.dim * 0.5f);
				if (d != 0) {
					if (_bounce[i]) {										
						if ( (d & 4) == 4 || (d & 8) == 8) {
							_velocities[i].y *= -1.0f;
						}
						if ((d & 1) == 1 || (d & 2) == 2) {
							_velocities[i].x *= -1.0f;
						}
						int t = _array->get<int>(_ids[i], WEC_TYPE);
						buffer.add(_ids[i], AT_BOUNCE, t, &_velocities[i], sizeof(v3));
						rotateTo(i);
						p += _velocities[i] * dt;// *1.5f;
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