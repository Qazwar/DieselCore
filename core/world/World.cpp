#include "World.h"
#include "actions\ScalingAction.h"
#include "actions\RemoveAfterAction.h"
#include "actions\ScaleByPathAction.h"
#include "actions\MoveByAction.h"
#include "actions\MoveByFiniteAction.h"
#include "actions\RotateByAction.h"
#include "actions\RotateToTargetAction.h"
#include "..\profiler\Profiler.h"
#include "actions\CollisionAction.h"

namespace ds {

	World::World() {
		for (int i = 0; i < 32; ++i) {
			_actions[i] = 0;
		}
		_collisionAction = 0;
		_data = new ChannelArray;
		int sizes[] = { sizeof(v3), sizeof(v3), sizeof(v3) ,sizeof(Texture) , sizeof(Color), sizeof(float), sizeof(int)};
		_data->init(sizes, 7);
	}


	World::~World()	{
		for (int i = 0; i < 32; ++i) {
			if (_actions[i] != 0) {
				delete _actions[i];
			}
		}
		if (_collisionAction != 0) {
			delete _collisionAction;
		}
		delete _data;
	}

	ID World::create() {
		return _data->add();
	}

	bool World::contains(ID id) const {
		return _data->contains(id);
	}

	ID World::create(const v2& pos, const Texture& texture, int type, float rotation, const v2& scale, const Color& color) {
		ID id = _data->add();
		_data->set<v3>(id, WEC_POSITION, v3(pos));
		_data->set<Texture>(id, WEC_TEXTURE, texture);
		_data->set<v3>(id, WEC_ROTATION, v3(rotation,0.0f,0.0f));
		_data->set<v3>(id, WEC_SCALE, v3(scale.x, scale.y, 1.0f));
		_data->set<Color>(id, WEC_COLOR, color);
		_data->set<int>(id, WEC_TYPE, type);
		return id;
	}

	uint32_t World::size() const {
		return _data->size;
	}

	void World::setRotation(ID id, const v3& rotation) {
		_data->set<v3>(id, WEC_ROTATION, rotation);
	}

	void World::setRotation(ID id, float rotation) {
		_data->set<v3>(id, WEC_ROTATION, v3(rotation));
	}

	void World::setColor(ID id, const Color& color) {
		_data->set<Color>(id, WEC_COLOR, color);
	}

	void World::setPosition(ID id, const v2& pos) {
		_data->set<v3>(id, WEC_POSITION, v3(pos));
	}

	const v3& World::getPosition(ID id) const {
		return _data->get<v3>(id, WEC_POSITION);
	}

	int World::getType(ID id) const {
		return _data->get<int>(id, WEC_TYPE);
	}

	const v3& World::getRotation(ID id) const {
		return _data->get<v3>(id, WEC_ROTATION);
	}

	void World::scaleByPath(ID id, V3Path* path, float ttl) {
		if (_actions[AT_SCALE_BY_PATH] == 0) {
			_actions[AT_SCALE_BY_PATH] = new ScaleByPathAction(_data);
		}
		ScaleByPathAction* action = (ScaleByPathAction*)_actions[AT_SCALE_BY_PATH];
		action->attach(id, path, ttl);
	}

	void World::scale(ID id, const v3& start, const v3& end, float ttl, int mode, const tweening::TweeningType& tweeningType) {
		if (_actions[AT_SCALE] == 0) {
			_actions[AT_SCALE] = new ScalingAction(_data);
		}
		ScalingAction* action = (ScalingAction*)_actions[AT_SCALE];
		action->attach(id, WEC_SCALE, start, end, ttl, mode, tweeningType);
	}

	// -----------------------------------------------
	// remove
	// -----------------------------------------------
	void World::remove(ID id) {
		for (int i = 0; i < 32; ++i) {
			if (_actions[i] != 0) {
				_actions[i]->removeByID(id);
			}
		}
		_collisionAction->removeByID(id);
		_additionalData.remove(id);
		_data->remove(id);
	}

	int World::find_by_type(int type, ID* ids, int max) const {
		int* indices = _data->_sparse;
		int cnt = 0;
		for (int i = 0; i < _data->capacity; ++i) {
			if (indices[i] != -1 && cnt < max) {
				int t = _data->get<int>(i, WEC_TYPE);
				if (t == type) {
					ids[cnt++] = i;
				}
			}
		}
		return cnt;
	}

	void World::attachCollider(ID id, ShapeType type, const v2& extent) {
		if (_collisionAction == 0) {
			_collisionAction = new CollisionAction(_data);
		}
		_collisionAction->attach(id, type, v3(extent));
	}

	bool World::hasCollisions() const {
		return _collisionAction->hasCollisions();
	}

	const Collision& World::getCollision(int idx) const {
		return _collisionAction->getCollision(idx);
	}

	uint32_t World::numCollisions() const {
		return _collisionAction->numCollisions();
	}

	void World::moveBy(ID id, const v2& velocity, bool bounce) {
		moveBy(id, v3(velocity), bounce);
	}

	void World::moveByFinite(ID id, const v3& velocity, float ttl, bool bounce) {
		if (_actions[AT_MOVE_BY_FINITE] == 0) {
			_actions[AT_MOVE_BY_FINITE] = new MoveByFiniteAction(_data);
		}
		MoveByFiniteAction* action = (MoveByFiniteAction*)_actions[AT_MOVE_BY_FINITE];
		action->attach(id, velocity, ttl, bounce);
	}

	void World::moveBy(ID id, const v3& velocity, bool bounce) {
		if (_actions[AT_MOVE_BY] == 0) {
			_actions[AT_MOVE_BY] = new MoveByAction(_data);
		}
		MoveByAction* action = (MoveByAction*)_actions[AT_MOVE_BY];
		action->attach(id, velocity, bounce);
	}

	void World::removeAfter(ID id, float ttl) {
		if (_actions[AT_REMOVE_AFTER] == 0) {
			_actions[AT_REMOVE_AFTER] = new RemoveAfterAction(_data);
		}
		RemoveAfterAction* action = (RemoveAfterAction*)_actions[AT_REMOVE_AFTER];
		action->attach(id, ttl);
	}

	void World::rotateTo(ID id, ID target, float angleVelocity) {
		if (_actions[AT_ROTATE_TO_TARGET] == 0) {
			_actions[AT_ROTATE_TO_TARGET] = new RotateToTargetAction(_data);
		}
		RotateToTargetAction* action = (RotateToTargetAction*)_actions[AT_ROTATE_TO_TARGET];
		action->attach(id, target, angleVelocity);
	}

	void World::rotateBy(ID id, float angle, float ttl) {
		if (_actions[AT_ROTATE_BY] == 0) {
			_actions[AT_ROTATE_BY] = new RotateByAction(_data);
		}
		RotateByAction* action = (RotateByAction*)_actions[AT_ROTATE_BY];
		action->attach(id, angle, ttl);
	}

	void World::stopAction(ID id, ActionType type) {
		if (_actions[type] != 0) {
			_actions[type]->removeByID(id);
		}
	}

	void World::tick(float dt) {
		_buffer.reset();
		{
			//ZoneTracker z2("World:tick:update");
			for (int i = 0; i < 32; ++i) {
				if (_actions[i] != 0) {
					_actions[i]->update(dt, _buffer);
				}
			}
			if (_collisionAction != 0) {
				_collisionAction->update(dt, _buffer);
			}
		}
		{
			//ZoneTracker z2("World:tick:kill");
			for (uint32_t i = 0; i < _buffer.events.size(); ++i) {
				const ActionEvent& e = _buffer.events[i];
				if (e.action == AT_KILL) {
					remove(e.id);
				}
			}
		}
	}

	void World::saveReport(const ds::ReportWriter& writer) {
		writer.startBox("World");
		const char* OVERVIEW_HEADERS[] = { "ID", "Index", "Position", "Texture", "Rotation", "Scale", "Color", "Type" };
		writer.startTable(OVERVIEW_HEADERS, 8);
		int* indices = _data->_sparse;
		for (int i = 0; i < _data->capacity; ++i) {
			if (indices[i] != -1) {
				writer.startRow();
				writer.addCell(i);
				writer.addCell(indices[i]);
				writer.addCell(_data->get<v3>(indices[i], WEC_POSITION));
				writer.addCell(_data->get<Texture>(indices[i], WEC_TEXTURE));
				writer.addCell(RADTODEG(_data->get<v3>(indices[i], WEC_ROTATION).x));
				writer.addCell(_data->get<v3>(indices[i], WEC_SCALE));
				writer.addCell(_data->get<Color>(indices[i], WEC_COLOR));
				writer.addCell(_data->get<int>(indices[i], WEC_TYPE));
				writer.endRow();
			}
		}
		writer.endTable();
		writer.endBox();
		for (int i = 0; i < 32; ++i) {
			if (_actions[i] != 0) {
				_actions[i]->saveReport(writer);
			}
		}
		_collisionAction->saveReport(writer);
	}

}