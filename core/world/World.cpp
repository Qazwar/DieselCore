#include "World.h"
#include "actions\ScalingAction.h"
#include "..\profiler\Profiler.h"

namespace ds {

	World::World(int channels) {
		for (int i = 0; i < 32; ++i) {
			_actions[i] = 0;
		}
		_data = new ChannelArray;
		int sizes[] = { sizeof(v3), sizeof(v3), sizeof(v3) ,sizeof(Texture) , sizeof(Color), sizeof(float), sizeof(int), sizeof(v3), sizeof(v3)};
		_data->init(sizes, 9);
	}


	World::~World()	{
		delete _data;
	}

	ID World::create() {
		return _data->add();
	}

	ID World::create(const v2& pos, const Texture& texture, float rotation, const v2& scale, const Color& color) {
		ID id = _data->add();
		_data->set<v3>(id, WEC_POSITION, v3(pos));
		_data->set<Texture>(id, WEC_TEXTURE, texture);
		_data->set<v3>(id, WEC_ROTATION, v3(rotation,0.0f,0.0f));
		_data->set<v3>(id, WEC_SCALE, v3(scale.x, scale.y, 1.0f));
		_data->set<Color>(id, WEC_COLOR, color);
		return id;
	}

	uint32_t World::size() const {
		return _data->size;
	}

	void World::scale(ID id, const v3& start, const v3& end, float ttl, int mode, const tweening::TweeningType& tweeningType) {
		if (_actions[AT_SCALE] == 0) {
			_actions[AT_SCALE] = new ScalingAction(_data);
		}
		ScalingAction* action = (ScalingAction*)_actions[AT_SCALE];
		action->attach(id, WEC_SCALE, start, end, ttl, mode, tweeningType);
	}

	void World::remove(ID id) {

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
				if (e.type == AT_KILL) {
					remove(e.id);
				}
			}
		}
	}

}