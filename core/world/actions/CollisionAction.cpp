#include "CollisionAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\log\Log.h"
#include "..\..\physics\ColliderArray.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	CollisionAction::CollisionAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "collision") {
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
		LOGC("physics") << "attach collider - id: " << id << " index: " << idx << " type: " << type << " extent: " << DBG_V3(extent);
		_ids[idx] = id;
		_previous[idx] = _array->get<v3>(id, WEC_POSITION);
		_types[idx] = type;
		_extents[idx] = extent;
	}

	bool CollisionAction::containsCollision(const Collision& c) const {
		for (uint32_t i = 0; i < _collisions.size(); ++i) {
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

	bool CollisionAction::isSupported(int firstType, int secondType) {
		for (uint32_t i = 0; i < _ignores.size(); ++i) {
			const IgnoredCollision& ic = _ignores[i];
			if (ic.firstType == firstType && ic.secondType == secondType) {
				return false;
			}
			if (ic.firstType == secondType && ic.secondType == firstType) {
				return false;
			}
		}
		return true;
	}

	void CollisionAction::ignore(int firstType, int secondType) {
		IgnoredCollision ignored;
		ignored.firstType = firstType;
		ignored.secondType = secondType;
		_ignores.push_back(ignored);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void CollisionAction::update(float dt,ActionEventBuffer& buffer) {
		_collisions.clear();
		float dist = 0.0f;
		if (_buffer.size > 0) {
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				for (uint32_t j = i + 1; j < _buffer.size; ++j) {
					if (_ids[i] != _ids[j]) {
						Collision c;
						c.firstType = _array->get<int>(_ids[i], WEC_TYPE);
						c.secondType = _array->get<int>(_ids[j], WEC_TYPE);
						if (isSupported(c.firstType, c.secondType)) {
							if (intersects(i, j, &c)) {
								LOGC("physics") << "intersection between " << i << " ( id: " << _ids[i] << " type: " << c.firstType << ") and " << j << " ( id: " << _ids[j] << " type: " << c.secondType << ")";
								c.firstID = _ids[i];
								c.secondID = _ids[j];
								if (!containsCollision(c)) {
									_collisions.push_back(c);
								}
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
	bool CollisionAction::intersects(int firstIndex, int secondIndex, Collision* c) {
		ShapeType firstShape = _types[firstIndex];
		ShapeType secondShape = _types[secondIndex];
		const v3& fp = _array->get<v3>(_ids[firstIndex], WEC_POSITION);
		const v3& fe = _extents[firstIndex];
		const v3& sp = _array->get<v3>(_ids[secondIndex],WEC_POSITION);
		const v3& se = _extents[secondIndex];
		if (firstShape == PST_CIRCLE && secondShape == PST_CIRCLE) {
			float r1 = fe.x * 0.5f;
			float r2 = se.x * 0.5f;
			//float u0, u1;			
			if (physics::testCircleIntersection(fp.xy(), r1, sp.xy(), r2)) {
				v3 d = sp - fp;
				float l = length(d);
				c->distance = l / (r1 + r2);
				c->norm = normalize(d);
				c->firstPos = fp;
				c->secondPos = sp;
				return true;
			}

			/*
			if (physics::testCircleSweepIntersection(r1, fpp.xy(), fp.xy(), r2, spp.xy(), sp.xy(), &u0, &u1)) {
				LOG << "u0 : " << u0 << " u1: " << u1;
				if (u0 <= 0.0f && u1 <= 0.0f) {
					return false;
				}
				if (u0 >= 1.0f && u1 >= 1.0f) {
					return false;
				}
				//if (physics::testCircleIntersection(fp, r1, sp, r2)) {
				return true;
			}
			*/
		}
		else if (firstShape == PST_QUAD && secondShape == PST_QUAD) {
			if (physics::testBoxIntersection(fp, fe, sp, se)) {
				return true;
			}
		}
		return false;
	}

	void CollisionAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.startBox("CollisionAction");
			const char* OVERVIEW_HEADERS[] = { "ID", "ShapeType", "Type", "Extent"};
			writer.startTable(OVERVIEW_HEADERS, 4);
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(_ids[i]);
				writer.addCell(_types[i]);
				writer.addCell(_array->get<int>(_ids[i], WEC_TYPE));
				writer.addCell(_extents[i]);
				writer.endRow();
			}
			writer.endTable();
			writer.endBox();
		}
	}

}