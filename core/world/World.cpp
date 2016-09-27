#include "World.h"
#include "actions\ScalingAction.h"
#include "..\profiler\Profiler.h"

namespace ds {

	World::World(int channels) {
		_array = new MultiplexArray(channels);
		for (int i = 0; i < 32; ++i) {
			_actions[i] = 0;
		}
	}


	World::~World()	{
		delete _array;
	}

	ID World::create() {
		return _array->add();
	}

	v4* World::getChannel(int index) const {
		// assert(index >= 0 && index < _numChannels);
		return _array->getPtr(index);
	}

	uint32_t World::size() const {
		return _array->size();
	}

	void World::scale(ID id, int channel, const v4& start, const v4& end, float ttl, int mode, const tweening::TweeningType& tweeningType) {
		if (_actions[AT_SCALE] == 0) {
			_actions[AT_SCALE] = new ScalingAction(_array);
		}
		ScalingAction* action = (ScalingAction*)_actions[AT_SCALE];
		action->attach(id, channel, start, end, ttl, mode, tweeningType);
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