#include "CollisionAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\log\Log.h"
#include "..\..\physics\ColliderArray.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	CollisionAction::CollisionAction(ChannelArray* array) : AbstractAction(array,"collision") {
		int sizes[] = { sizeof(ID), sizeof(v3), sizeof(ShapeType), sizeof(v3) };
		_buffer.init(sizes, 4);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	CollisionAction::~CollisionAction() {}

	void CollisionAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_previous = (v3*)_buffer.get_ptr(1);
			_types = (ShapeType*)_buffer.get_ptr(2);
			_extents = (v3*)_buffer.get_ptr(3);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void CollisionAction::attach(ID id, ShapeType type, const v3& extent) {
		int idx = create(id);
		_ids[idx] = id;
		_previous[idx] = _array->get<v3>(id, WEC_POSITION);
		_types[idx] = type;
		_extents[idx] = extent;
	}

	bool CollisionAction::containsCollision(const Collision& c) const {
		for (int i = 0; i < _collisions.size(); ++i) {
			const Collision& other = _collisions[i];
			if (other.firstID == c.firstID && other.secondID == c.secondID) {
				return true;
			}
			if (other.firstID == c.secondID && other.secondID == c.firstID) {
				return true;
			}
		}
		return false;
	}


	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void CollisionAction::update(float dt,ActionEventBuffer& buffer) {
		_collisions.clear();
		if (_buffer.size > 0) {
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				for (uint32_t j = i + 1; j < _buffer.size; ++j) {
					if (i != j) {
						if (intersects(i, j)) {
							//LOG << "intersection between " << i << " (" << _ids[i] << ") and " << j << " (" << _ids[j] << ")";
							Collision c;
							c.firstPos = _array->get<v3>(_ids[i], WEC_POSITION);
							c.firstID = _ids[i];
							c.firstType = _array->get<int>(_ids[i], WEC_TYPE);
							c.secondPos = _array->get<v3>(_ids[j], WEC_POSITION);
							c.secondID = _ids[j];
							c.secondType = _array->get<int>(_ids[j], WEC_TYPE);
							if (!containsCollision(c)) {
								_collisions.push_back(c);
							}
						}
					}
				}
			}
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				_previous[i] = _array->get<v3>(_ids[i], WEC_POSITION);
			}
		}
	}

	// --------------------------------------------------------------------------
	// intersects
	// --------------------------------------------------------------------------
	bool CollisionAction::intersects(int firstIndex, int secondIndex) {
		ShapeType firstShape = _types[firstIndex];
		ShapeType secondShape = _types[secondIndex];
		const v3& fp = _array->get<v3>(_ids[firstIndex], WEC_POSITION);
		const v3& fpp = _previous[firstIndex];
		const v3& fe = _extents[firstIndex];
		const v3& sp = _array->get<v3>(_ids[secondIndex],WEC_POSITION);
		const v3& spp = _previous[secondIndex];
		const v3& se = _extents[secondIndex];
		if (firstShape == PST_CIRCLE && secondShape == PST_CIRCLE) {
			// FIXME: calculate radius based on scale of sprite
			float r1 = fe.x * 0.5f;
			float r2 = se.x * 0.5f;
			float u0, u1;
			if (physics::testCircleSweepIntersection(r1, fpp.xy(), fp.xy(), r2, spp.xy(), sp.xy(), &u0, &u1)) {
				//LOG << "u0 : " << u0 << " u1: " << u1;
				if (u0 <= 0.0f && u1 <= 0.0f) {
					return false;
				}
				if (u0 >= 1.0f && u1 >= 1.0f) {
					return false;
				}
				//if (physics::testCircleIntersection(fp, r1, sp, r2)) {
				return true;
			}
		}
		else if (firstShape == PST_QUAD && secondShape == PST_QUAD) {
			if (physics::testBoxIntersection(fp, fe, sp, se)) {
				return true;
			}
		}
		return false;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void CollisionAction::debug() {
		if ( _buffer.size > 0 ) {
			LOG << "---------- ScalingAction ---------- ";
		}
		/*
		std::map<SID,int>::iterator it = m_Mapping.begin();
		while ( it != m_Mapping.end()) {
			LOG << it->first << " = " << it->second;
			++it;
		}
		*/
	}

	void CollisionAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.startBox("ScalingAction");
			const char* OVERVIEW_HEADERS[] = { "ID", "ShapeType", "Extent"};
			writer.startTable(OVERVIEW_HEADERS, 3);
			for (int i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(_ids[i]);
				writer.addCell(_types[i]);
				writer.addCell(_extents[i]);
				writer.endRow();
			}
			writer.endTable();
			writer.endBox();
		}
	}

}