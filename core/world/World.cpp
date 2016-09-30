#include "World.h"
#include "actions\ScalingAction.h"
#include "actions\RemoveAfterAction.h"
#include "actions\ScaleByPathAction.h"
#include "..\profiler\Profiler.h"

namespace ds {

	World::World() {
		for (int i = 0; i < 32; ++i) {
			_actions[i] = 0;
		}
		_data = new ChannelArray;
		int sizes[] = { sizeof(v3), sizeof(v3), sizeof(v3) ,sizeof(Texture) , sizeof(Color), sizeof(float), sizeof(int), sizeof(v3), sizeof(v3)};
		_data->init(sizes, 9);
	}


	World::~World()	{
		for (int i = 0; i < 32; ++i) {
			if (_actions[i] != 0) {
				delete _actions[i];
			}
		}
		delete _data;
	}

	ID World::create() {
		return _data->add();
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

	void World::setPosition(ID id, const v2& pos) {
		_data->set<v3>(id, WEC_POSITION, v3(pos));
	}

	const v3& World::getPosition(ID id) const {
		return _data->get<v3>(id, WEC_POSITION);
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

	void World::remove(ID id) {
		for (int i = 0; i < 32; ++i) {
			if (_actions[i] != 0) {
				_actions[i]->removeByID(id);
			}
		}
		_data->remove(id);
	}

	int World::find_by_type(int type, ID* ids, int max) const {
		Index* indices = _data->_data_indices;
		int cnt = 0;
		for (uint32_t i = 0; i < _data->capacity; ++i) {
			const Index& in = indices[i];
			if (in.index != USHRT_MAX && cnt < max) {
				int t = _data->get<int>(in.id, WEC_TYPE);
				if (t == type) {
					ids[cnt++] = in.id;
				}
			}
		}
		return cnt;
	}

	void World::removeAfter(ID id, float ttl) {
		if (_actions[AT_REMOVE_AFTER] == 0) {
			_actions[AT_REMOVE_AFTER] = new RemoveAfterAction(_data);
		}
		RemoveAfterAction* action = (RemoveAfterAction*)_actions[AT_REMOVE_AFTER];
		action->attach(id, ttl);
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
		}
		{
			//ZoneTracker z2("World:tick:kill");
			for (int i = 0; i < _buffer.events.size(); ++i) {
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
		Index* indices = _data->_data_indices;
		for (uint32_t i = 0; i < _data->capacity; ++i) {
			const Index& in = indices[i];
			if (in.index != USHRT_MAX) {
				writer.startRow();
				writer.addCell(in.id);
				writer.addCell(in.index);
				writer.addCell(_data->get<v3>(in.id, WEC_POSITION));
				writer.addCell(_data->get<Texture>(in.id, WEC_TEXTURE));
				writer.addCell(RADTODEG(_data->get<v3>(in.id, WEC_ROTATION).x));
				writer.addCell(_data->get<v3>(in.id, WEC_SCALE));
				writer.addCell(_data->get<Color>(in.id, WEC_COLOR));
				writer.addCell(_data->get<int>(in.id, WEC_TYPE));
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
	}

}