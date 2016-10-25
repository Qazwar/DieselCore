#include "World.h"
#include "actions\ScalingAction.h"
#include "actions\RemoveAfterAction.h"
#include "actions\ScaleByPathAction.h"
#include "actions\MoveByAction.h"
#include "actions\RotateByAction.h"
#include "actions\RotateToTargetAction.h"
#include "actions\LookAtAction.h"
#include "actions\SeekAction.h"
#include "actions\SeparateAction.h"
#include "..\profiler\Profiler.h"
#include "actions\CollisionAction.h"
#include "actions\AlphaFadeToAction.h"

namespace ds {

	World::World() : _boundingRect(0,0,1024,768) {
		for (int i = 0; i < 32; ++i) {
			_actions[i] = 0;
		}
		_collisionAction = 0;
		_data = new ChannelArray;
		int sizes[] = { sizeof(v3), sizeof(v3), sizeof(v3) ,sizeof(Texture) , sizeof(Color), sizeof(float), sizeof(int), sizeof(v3)};
		_data->init(sizes, 8);
		_templates = 0;
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

	// -----------------------------------------------
	// set world dimension
	// -----------------------------------------------
	void World::setWorldDimension(const v2& dim) {
		_boundingRect = Rect(0.0f, 0.0f, dim.x, dim.y);
	}

	void World::setBoundingRect(const Rect& r) {
		_boundingRect = r;
	}

	void World::useTemplates(WorldEntityTemplates* templates) {
		_templates = templates;
	}
	// -----------------------------------------------
	// create
	// -----------------------------------------------
	ID World::create() {
		return _data->add();
	}

	// -----------------------------------------------
	// contains
	// -----------------------------------------------
	bool World::contains(ID id) const {
		return _data->contains(id);
	}

	// -----------------------------------------------
	// create
	// -----------------------------------------------
	ID World::create(const v2& pos, const Texture& texture, int type, float rotation, const v2& scale, const Color& color) {
		ID id = _data->add();
		//LOGC("world") << "create - id: " << id;
		_data->set<v3>(id, WEC_POSITION, v3(pos));
		_data->set<Texture>(id, WEC_TEXTURE, texture);
		_data->set<v3>(id, WEC_ROTATION, v3(rotation,0.0f,0.0f));
		_data->set<v3>(id, WEC_SCALE, v3(scale.x, scale.y, 1.0f));
		_data->set<Color>(id, WEC_COLOR, color);
		_data->set<int>(id, WEC_TYPE, type);
		_data->set<v3>(id, WEC_FORCE, v3(0.0f));
		return id;
	}

	ID World::create(const v2& pos, StaticHash entityHash) {
		assert(_templates != 0);
		int eidx = _templates->findIndex(entityHash);
		assert(eidx != -1);
		const WorldEntity& tpl = _templates->getTemplate(eidx);
		return create(pos, tpl.texture, tpl.type, tpl.rotation.x, tpl.scale.xy(), tpl.color);
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

	const v3& World::getScale(ID id) const {
		return _data->get<v3>(id, WEC_SCALE);
	}

	void World::setScale(ID id, const v3& s) {
		_data->set<v3>(id, WEC_SCALE, s);
	}
	// -----------------------------------------------
	// scale by path
	// -----------------------------------------------
	void World::scaleByPath(ID id, V3Path* path, float ttl) {
		if (_actions[AT_SCALE_BY_PATH] == 0) {
			_actions[AT_SCALE_BY_PATH] = new ScaleByPathAction(_data, _boundingRect);
		}
		ScaleByPathAction* action = (ScaleByPathAction*)_actions[AT_SCALE_BY_PATH];
		action->attach(id, path, ttl);
	}

	// -----------------------------------------------
	// scale
	// -----------------------------------------------
	void World::scale(ID id, const v3& start, const v3& end, float ttl, int mode, const tweening::TweeningType& tweeningType) {
		if (_actions[AT_SCALE] == 0) {
			_actions[AT_SCALE] = new ScalingAction(_data, _boundingRect);
		}
		ScalingAction* action = (ScalingAction*)_actions[AT_SCALE];
		action->attach(id, WEC_SCALE, start, end, ttl, mode, tweeningType);
	}

	// -----------------------------------------------
	// remove
	// -----------------------------------------------
	void World::remove(ID id) {
		//LOGC("world") << "removing: " << id;
		if (_data->contains(id)) {
			for (int i = 0; i < 32; ++i) {
				if (_actions[i] != 0) {
					_actions[i]->removeByID(id);
				}
			}
			for (uint32_t i = 0; i < _customActions.size(); ++i) {
				_customActions[i]->removeByID(id);
			}
			_collisionAction->removeByID(id);
			_additionalData.remove(id);
			_data->remove(id);
		}
		else {
			LOGE << "requesting to remove " << id << " but it is not part of the world";
		}
	}

	// -----------------------------------------------
	// find by type
	// -----------------------------------------------
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

	// -----------------------------------------------
	// attach collider
	// -----------------------------------------------
	void World::attachCollider(ID id, ShapeType type, const v2& extent) {
		if (_collisionAction == 0) {
			_collisionAction = new CollisionAction(_data, _boundingRect);
		}
		_collisionAction->attach(id, type, v3(extent));
	}

	void World::ignoreCollisions(int firstType, int secondType) {
		if (_collisionAction == 0) {
			_collisionAction = new CollisionAction(_data, _boundingRect);
		}
		_collisionAction->ignore(firstType, secondType);
	}

	// -----------------------------------------------
	// has collisions
	// -----------------------------------------------
	bool World::hasCollisions() const {
		return _collisionAction->hasCollisions();
	}

	// -----------------------------------------------
	// get collision
	// -----------------------------------------------
	const Collision& World::getCollision(int idx) const {
		return _collisionAction->getCollision(idx);
	}

	// -----------------------------------------------
	// num collisions
	// -----------------------------------------------
	uint32_t World::numCollisions() const {
		return _collisionAction->numCollisions();
	}

	// -----------------------------------------------
	// move by (2D)
	// -----------------------------------------------
	void World::moveBy(ID id, const v2& velocity, float ttl, bool bounce) {
		moveBy(id, v3(velocity), ttl, bounce);
	}

	// -----------------------------------------------
	// move by
	// -----------------------------------------------
	void World::moveBy(ID id, const v3& velocity, float ttl, bool bounce) {
		if (_actions[AT_MOVE_BY] == 0) {
			_actions[AT_MOVE_BY] = new MoveByAction(_data, _boundingRect);
		}
		MoveByAction* action = (MoveByAction*)_actions[AT_MOVE_BY];
		action->attach(id, velocity, ttl, bounce);
	}

	// -----------------------------------------------
	// remove after
	// -----------------------------------------------
	void World::removeAfter(ID id, float ttl) {
		if (_actions[AT_REMOVE_AFTER] == 0) {
			_actions[AT_REMOVE_AFTER] = new RemoveAfterAction(_data, _boundingRect);
		}
		RemoveAfterAction* action = (RemoveAfterAction*)_actions[AT_REMOVE_AFTER];
		action->attach(id, ttl);
	}

	void World::alphaFadeTo(ID id, float start, float end, float ttl) {
		if (_actions[AT_ALPHA_FADE_TO] == 0) {
			_actions[AT_ALPHA_FADE_TO] = new AlphaFadeToAction(_data, _boundingRect);
		}
		AlphaFadeToAction* action = (AlphaFadeToAction*)_actions[AT_ALPHA_FADE_TO];
		action->attach(id, start, end, ttl);
	}
	// -----------------------------------------------
	// separate
	// -----------------------------------------------
	void World::separate(ID id, int type, float minDistance, float relaxation) {
		if (_actions[AT_SEPARATE] == 0) {
			_actions[AT_SEPARATE] = new SeparateAction(_data, _boundingRect);
		}
		SeparateAction* action = (SeparateAction*)_actions[AT_SEPARATE];
		action->attach(id, type, minDistance, relaxation);
	}

	// -----------------------------------------------
	// seek
	// -----------------------------------------------
	void World::seek(ID id, ID target, float velocity) {
		if (_actions[AT_SEEK] == 0) {
			_actions[AT_SEEK] = new SeekAction(_data, _boundingRect);
		}
		SeekAction* action = (SeekAction*)_actions[AT_SEEK];
		action->attach(id, target, velocity);
	}

	// -----------------------------------------------
	// look at
	// -----------------------------------------------
	void World::lookAt(ID id, ID target, float ttl) {
		if (_actions[AT_LOOK_AT] == 0) {
			_actions[AT_LOOK_AT] = new LookAtAction(_data, _boundingRect);
		}
		LookAtAction* action = (LookAtAction*)_actions[AT_LOOK_AT];
		action->attach(id, target, ttl);
	}

	// -----------------------------------------------
	// rotate to
	// -----------------------------------------------
	void World::rotateTo(ID id, ID target, float angleVelocity) {
		if (_actions[AT_ROTATE_TO_TARGET] == 0) {
			_actions[AT_ROTATE_TO_TARGET] = new RotateToTargetAction(_data, _boundingRect);
		}
		RotateToTargetAction* action = (RotateToTargetAction*)_actions[AT_ROTATE_TO_TARGET];
		action->attach(id, target, angleVelocity);
	}

	// -----------------------------------------------
	// rotate by
	// -----------------------------------------------
	void World::rotateBy(ID id, float angle, float ttl) {
		if (_actions[AT_ROTATE_BY] == 0) {
			_actions[AT_ROTATE_BY] = new RotateByAction(_data, _boundingRect);
		}
		RotateByAction* action = (RotateByAction*)_actions[AT_ROTATE_BY];
		action->attach(id, angle, ttl);
	}

	// -----------------------------------------------
	// stop action
	// -----------------------------------------------
	void World::stopAction(ID id, ActionType type) {
		if (_actions[type] != 0) {
			_actions[type]->removeByID(id);
		}
	}

	// -----------------------------------------------
	// tick
	// -----------------------------------------------
	void World::tick(float dt) {
		ZoneTracker("World::tick");
		_buffer.reset();
		// reset forces
		v3* forces = (v3*)_data->get_ptr(WEC_FORCE);		
		for (uint32_t i = 0; i < _data->size; ++i) {
			*forces = v3(0.0f);
			++forces;
		}
		// update all actions
		{
			ZoneTracker("World::tick::update");
			for (int i = 0; i < 32; ++i) {
				if (_actions[i] != 0) {
					_actions[i]->update(dt, _buffer);
				}
			}
		}
		// update all custom actions
		{
			ZoneTracker("World::tick::updateCustom");
			for (uint32_t i = 0; i < _customActions.size(); ++i) {
				_customActions[i]->update(dt, _buffer);
			}
		}
		// apply forces
		{
			ZoneTracker("World::tick::applyForces");
			forces = (v3*)_data->get_ptr(WEC_FORCE);
			v3* positions = (v3*)_data->get_ptr(WEC_POSITION);
			for (uint32_t i = 0; i < _data->size; ++i) {
				*positions += *forces;
				++forces;
				++positions;
			}
		}
		// handle collisions
		{
			ZoneTracker("World::tick::collisions");
			if (_collisionAction != 0) {
				_collisionAction->update(dt, _buffer);
			}
		}
		// process events
		{
			ZoneTracker("World::tick::events");
			for (uint32_t i = 0; i < _buffer.events.size(); ++i) {
				const ActionEvent& e = _buffer.events[i];
				if (e.action == AT_KILL) {
					remove(e.id);
				}
			}
		}
	}

	// -----------------------------------------------
	// save report
	// -----------------------------------------------
	void World::saveReport(const ds::ReportWriter& writer) {
		writer.startBox("World");
		const char* OVERVIEW_HEADERS[] = { "ID", "Index", "Position", "Texture", "Rotation", "Scale", "Color", "Type", "Force" };
		writer.startTable(OVERVIEW_HEADERS, 9);
		int* indices = _data->_sparse;
		for (int i = 0; i < _data->capacity; ++i) {
			if (indices[i] != -1) {
				writer.startRow();
				writer.addCell(i);
				writer.addCell(indices[i]);
				writer.addCell(_data->get<v3>(i, WEC_POSITION));
				writer.addCell(_data->get<Texture>(i, WEC_TEXTURE));
				writer.addCell(RADTODEG(_data->get<v3>(i, WEC_ROTATION).x));
				writer.addCell(_data->get<v3>(i, WEC_SCALE));
				writer.addCell(_data->get<Color>(i, WEC_COLOR));
				writer.addCell(_data->get<int>(i, WEC_TYPE));
				writer.addCell(_data->get<v3>(i, WEC_FORCE));
				writer.endRow();
			}
		}
		writer.endTable();
		writer.endBox();
		_additionalData.save(writer);
		for (int i = 0; i < 32; ++i) {
			if (_actions[i] != 0) {
				_actions[i]->saveReport(writer);
			}
		}
		for (uint32_t i = 0; i < _customActions.size(); ++i) {
			_customActions[i]->saveReport(writer);
		}
		_collisionAction->saveReport(writer);
	}

}