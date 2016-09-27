#pragma once
#include "DataArray.h"
#include <Vector.h>

const int MAX_BLOCKS = 16;

struct BlockArray {

	char* data;
	uint32_t size;
	uint32_t capacity;
	uint32_t total_capacity;
	int _sizes[MAX_BLOCKS];
	int _indices[MAX_BLOCKS];
	int _num_blocks;

	BlockArray();

	~BlockArray();

	void init(int* sizes, int num);

	void* get_ptr(int index);

	bool resize(int new_size);

	void remove(int index);

	void swap(int oldIndex, int newIndex);
	
};

namespace ds {

	class MultiplexArray {

	public:
		MultiplexArray(uint32_t channels);
		~MultiplexArray();
		ID add();
		void set(ID id, int channel, float v);
		void set(ID id, int channel, const v2& v);
		void set(ID id, int channel, const v3& v);
		void set(ID id, int channel, const v4& v);
		const v4& get(ID id, int channel) const;
		void remove(ID id);
		bool contains(ID id);
		v4* getPtr(int channel) const;
		uint32_t size() const {
			return _size;
		}
	private:
		int find(int data_index) const;
		bool resize(int new_size);
		Index* _indices;
		v4* _data;
		uint32_t _channels;
		uint32_t _size;
		uint32_t _capacity;
		uint32_t _total;
		unsigned short _free_enqueue;
		unsigned short _free_dequeue;
	};

	struct ChannelArray {

		char* data;
		int size;
		int capacity;
		int total_capacity;
		int _sizes[MAX_BLOCKS];
		int _indices[MAX_BLOCKS];
		int _num_blocks;
		Index* _data_indices;
		unsigned short _free_enqueue;
		unsigned short _free_dequeue;

		ChannelArray();

		~ChannelArray();

		void init(int* sizes, int num);

		ID add();

		template<class T>
		void set(ID id, int channel, const T& t) {
			T* p = (T*)get_ptr(channel);
			assert(id != UINT_MAX);
			unsigned short index = _data_indices[id & INDEX_MASK].index;
			assert(index != USHRT_MAX);
			p[index] = t;
		}

		const bool contains(ID id) const {
			const Index& in = _data_indices[id & INDEX_MASK];
			return in.id == id && in.index != USHRT_MAX;
		}

		void remove(ID id);

		template<class T>
		const T& get(ID id, int channel) const {
			const T* p = get_ptr(channel);
			assert(id != UINT_MAX);
			unsigned short index = _data_indices[id & INDEX_MASK].index;
			return p[index];
		}

		template<class T>
		T& get(ID id, int channel) {
			T* p = (T*)get_ptr(channel);
			assert(id != UINT_MAX);
			unsigned short index = _data_indices[id & INDEX_MASK].index;
			return p[index];
		}

		void* get_ptr(int channel);

		bool resize(int new_size);

	};

}
