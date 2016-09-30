#include "BlockArray.h"
#include <string.h>
#include "..\memory\DefaultAllocator.h"
#include "..\log\Log.h"

BlockArray::BlockArray() : size(0), capacity(0), data(0) , total_capacity(0) {
}


BlockArray::~BlockArray() {
	if (data != 0) {
		DEALLOC(data);
	}
}

void BlockArray::init(int* sizes, int num) {
	assert(num < MAX_BLOCKS);
	for (int i = 0; i < num; ++i) {
		_sizes[i] = sizes[i];
	}
	_num_blocks = num;
}

bool BlockArray::resize(uint32_t new_size) {
	if (new_size > capacity) {
		new_size = capacity * 2 + new_size;
		int total = 0;
		for (int i = 0; i < _num_blocks; ++i) {
			total += _sizes[i];
		}
		
		int sz = new_size * total;
		char* t = (char*)ALLOC(sz);
		if (data != 0) {
			int offset = 0;
			int old_offset = 0;
			for (int i = 0; i < _num_blocks; ++i) {
				memcpy(t + offset, data + old_offset, size * _sizes[i]);
				offset += new_size * _sizes[i];
				old_offset += capacity * _sizes[i];
			}
			DEALLOC(data);
		}
		capacity = new_size;
		_indices[0] = 0;
		for (int i = 1; i < _num_blocks; ++i) {
			_indices[i] = _indices[i - 1] + _sizes[i- 1] * capacity;
		}
		data = t;
		return true;
	}
	return false;
}

void* BlockArray::get_ptr(int index) {
	return data + _indices[index];
}

void BlockArray::remove(int index) {
	int last = size - 1;
	for (int i = 0; i < _num_blocks; ++i) {
		char* dest = data + _indices[i] + index * _sizes[i];
		char* src = data + _indices[i] + last * _sizes[i];
		memcpy(dest, src, _sizes[i]);
	}
	--size;
}

void BlockArray::swap(int newIndex,int oldIndex) {
	for (int i = 0; i < _num_blocks; ++i) {
		char* dest = data + _indices[i] + newIndex * _sizes[i];
		char* src = data + _indices[i] + oldIndex * _sizes[i];
		memcpy(dest, src, _sizes[i]);
	}
}

namespace ds {

	// --------------------------------------------------------
	// MultiplexArray
	// --------------------------------------------------------

	MultiplexArray::MultiplexArray(uint32_t channels) {
		_indices = 0;
		_data = 0;
		_channels = channels;
		_size = 0;
		_capacity = 0;
		_total = 0;
		_free_enqueue = 0;
		_free_dequeue = 0;
	}

	MultiplexArray::~MultiplexArray() {
		if (_indices != 0) {
			DEALLOC(_indices);
		}
		if (_data != 0) {
			DEALLOC(_data);
		}
	}

	// -----------------------------------------------
	// add
	// -----------------------------------------------
	ID MultiplexArray::add() {
		if (_size + 1 > _capacity) {
			resize(_size * 2 + 8);
		}
		Index &in = _indices[_free_dequeue];
		_free_dequeue = in.next;
		in.index = _size++;
		for (uint32_t i = 0; i < _channels; ++i) {
			_data[in.index] = v4(0.0f);
		}
		return in.id;
	}

	// -----------------------------------------------
	// set 
	// -----------------------------------------------
	void MultiplexArray::set(ID id, int channel, float v) {
		set(id, channel, v4(v, 0.0f, 0.0f, 0.0f));
	}

	// -----------------------------------------------
	// set 
	// -----------------------------------------------
	void MultiplexArray::set(ID id, int channel, const v2& v) {
		set(id, channel, v4(v.x, v.y, 0.0f, 0.0f));
	}

	// -----------------------------------------------
	// set 
	// -----------------------------------------------
	void MultiplexArray::set(ID id, int channel, const v3& v) {
		set(id, channel, v4(v.x, v.y, v.z, 0.0f));
	}

	// -----------------------------------------------
	// set 
	// -----------------------------------------------
	void MultiplexArray::set(ID id, int channel, const v4& v) {
		v4* p = getPtr(channel);
		assert(id != UINT_MAX);
		unsigned short index = _indices[id & INDEX_MASK].index;
		assert(index != USHRT_MAX);
		p[index] = v;
	}

	// -----------------------------------------------
	// get
	// -----------------------------------------------
	const v4& MultiplexArray::get(ID id, int channel) const {
		v4* p = getPtr(channel);
		assert(id != UINT_MAX);
		unsigned short index = _indices[id & INDEX_MASK].index;
		return p[index];
	}

	// -----------------------------------------------
	// find indices index by data index
	// -----------------------------------------------
	int MultiplexArray::find(int data_index) const {
		for (uint32_t i = 0; i < _capacity; ++i) {
			if (_indices[i].index == data_index) {
				return i;
			}
		}
		return -1;
	}

	// -----------------------------------------------
	// remove by id
	// -----------------------------------------------
	void MultiplexArray::remove(ID id) {
		Index &in = _indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		int l = find(_size - 1);
		if (l != -1) {
			Index& last = _indices[l];
			int current = in.index;
			int next = last.index;
			for (uint32_t i = 0; i < _channels; ++i) {
				_data[current] = _data[next];
				current += _capacity;
				next += _capacity;
			}
			last.index = in.index;
		}
		--_size;		
		//_indices[in.id & INDEX_MASK].index = in.index;
		in.index = USHRT_MAX;
		_indices[_free_enqueue].next = id & INDEX_MASK;
		_free_enqueue = id & INDEX_MASK;
	}

	// -----------------------------------------------
	// contains id
	// -----------------------------------------------
	bool MultiplexArray::contains(ID id) {
		Index &in = _indices[id & INDEX_MASK];
		return (in.index != USHRT_MAX);
	}

	// -----------------------------------------------
	// get pointer to channel
	// -----------------------------------------------
	v4* MultiplexArray::getPtr(int channel) const {
		return _data + channel * _capacity;
	}

	// -----------------------------------------------
	// resize
	// -----------------------------------------------
	bool MultiplexArray::resize(uint32_t new_size) {
		if (new_size > _capacity) {
			if (_indices == 0) {
				_indices = (Index*)ALLOC(new_size * sizeof(Index));
				for (unsigned short i = 0; i < new_size; ++i) {
					_indices[i].id = i;
					_indices[i].next = i + 1;
				}
				_free_dequeue = 0;
				_free_enqueue = new_size - 1;
			}
			else {
				Index* tmp = (Index*)ALLOC(new_size * sizeof(Index));
				memcpy(tmp, _indices, _size * sizeof(Index));
				for (unsigned short i = _size; i < new_size; ++i) {
					tmp[i].id = i;
					tmp[i].next = i + 1;
				}
				DEALLOC(_indices);
				_indices = tmp;
				_free_enqueue = new_size - 1;
			}
			int sz = new_size * _channels * sizeof(v4);
			v4* t = (v4*)ALLOC(sz);
			if (_data != 0) {
				int offset = 0;
				for (uint32_t i = 0; i < _channels; ++i) {
					memcpy(t + i * new_size, _data + offset, _size * sizeof(v4));
					offset += _capacity;
				}
				DEALLOC(_data);
			}
			_capacity = new_size;
			_data = t;
			return true;
		}
		return false;
	}


	// --------------------------------------------------------
	// ChannelArray
	// --------------------------------------------------------

	ChannelArray::ChannelArray() : size(0), capacity(0), data(0), total_capacity(0) , _data_indices(0) , _free_dequeue(0) , _free_enqueue(0) {
	}


	ChannelArray::~ChannelArray() {
		if (_data_indices != 0) {
			DEALLOC(_data_indices);
		}
		if (data != 0) {
			DEALLOC(data);
		}
	}

	// -----------------------------------------------
	// init by channel types
	// -----------------------------------------------
	void ChannelArray::init(ChannelType* types, int num) {
		assert(num < MAX_BLOCKS);
		for (int i = 0; i < num; ++i) {
			const ChannelDefinition& def = CHANNEL_DEFINITIONS[types[i]];
			_sizes[i] = def.size;
			_types[i] = def.type;
		}
		_num_blocks = num;
	}

	// -----------------------------------------------
	// init
	// -----------------------------------------------
	void ChannelArray::init(int* sizes, int num) {
		assert(num < MAX_BLOCKS);
		for (int i = 0; i < num; ++i) {
			_sizes[i] = sizes[i];
		}
		_num_blocks = num;
	}

	// -----------------------------------------------
	// add
	// -----------------------------------------------
	ID ChannelArray::add() {
		if (size + 1 > capacity) {
			resize(size * 2 + 8);
		}
		Index &in = _data_indices[_free_dequeue];
		_free_dequeue = in.next;
		in.index = size++;
		return in.id;
	}

	// -----------------------------------------------
	// resize
	// -----------------------------------------------
	bool ChannelArray::resize(int new_size) {
		if (new_size > capacity) {
			int total = 0;
			for (int i = 0; i < _num_blocks; ++i) {
				total += _sizes[i];
			}
			if (_data_indices == 0) {
				_data_indices = (Index*)ALLOC(new_size * sizeof(Index));
				for (unsigned short i = 0; i < new_size; ++i) {
					_data_indices[i].id = i;
					_data_indices[i].next = i + 1;
					_data_indices[i].index = USHRT_MAX;
				}
				_free_dequeue = 0;
				_free_enqueue = new_size - 1;
			}
			else {
				Index* tmp = (Index*)ALLOC(new_size * sizeof(Index));
				memcpy(tmp, _data_indices, size * sizeof(Index));
				for (unsigned short i = size; i < new_size; ++i) {
					tmp[i].id = i;
					tmp[i].next = i + 1;
					tmp[i].index = USHRT_MAX;
				}
				DEALLOC(_data_indices);
				_data_indices = tmp;
				_free_enqueue = new_size - 1;
			}
			int sz = new_size * total;
			char* t = (char*)ALLOC(sz);
			if (data != 0) {
				int offset = 0;
				int old_offset = 0;
				for (int i = 0; i < _num_blocks; ++i) {
					memcpy(t + offset, data + old_offset, size * _sizes[i]);
					offset += new_size * _sizes[i];
					old_offset += capacity * _sizes[i];
				}
				DEALLOC(data);
			}
			capacity = new_size;
			_indices[0] = 0;
			for (int i = 1; i < _num_blocks; ++i) {
				_indices[i] = _indices[i - 1] + _sizes[i - 1] * capacity;
			}
			data = t;
			return true;
		}
		return false;
	}

	// -----------------------------------------------
	// get pointer to channel
	// -----------------------------------------------
	void* ChannelArray::get_ptr(int index) {
		assert(index >= 0 && index < MAX_BLOCKS);
		return data + _indices[index];
	}

	// -----------------------------------------------
	// get pointer to channel
	// -----------------------------------------------
	void* ChannelArray::getPointer(int channel, ChannelType type) {
		assert(channel >= 0 && channel < MAX_BLOCKS);
		assert(_types[channel] == type);
		return data + _indices[channel];
	}

	// -----------------------------------------------
	// find indices index by data index
	// -----------------------------------------------
	int ChannelArray::find(int data_index) const {
		for (int i = 0; i < capacity; ++i) {
			if (_data_indices[i].index == data_index) {
				return i;
			}
		}
		return -1;
	}

	// -----------------------------------------------
	// remove
	// -----------------------------------------------
	void ChannelArray::remove(ID id) {
		Index &in = _data_indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		if (size > 0) {
			int l = find(size - 1);
			if (l != -1 && l != in.id) {
				Index& last = _data_indices[l];
				for (int i = 0; i < _num_blocks; ++i) {
					int current = _indices[i] + in.index * _sizes[i];
					int next = _indices[i] + last.index * _sizes[i];
					memcpy(data + current, data + next, _sizes[i]);					
				}
				last.index = in.index;
			}
		}
		--size;
		in.index = USHRT_MAX;
		_data_indices[_free_enqueue].next = id & INDEX_MASK;
		_free_enqueue = id & INDEX_MASK;
	}

}
