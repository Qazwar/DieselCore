#pragma once
#include "DataArray.h"
#include <Vector.h>
#include "..\graphics\Color.h"
#include "..\graphics\Texture.h"
#include "..\math\tweening.h"

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

	bool resize(uint32_t new_size);

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
		bool resize(uint32_t new_size);
		Index* _indices;
		v4* _data;
		uint32_t _channels;
		uint32_t _size;
		uint32_t _capacity;
		uint32_t _total;
		unsigned short _free_enqueue;
		unsigned short _free_dequeue;
	};

	enum ChannelType {
		CT_INT,CT_FLOAT,CT_V2,CT_V3,CT_V4,CT_COLOR,CT_RECT,CT_ID,CT_TWN
	};

	struct ChannelDefinition {
		ChannelType type;
		int size;

		ChannelDefinition(ChannelType t, int s) : type(t), size(s) {}
	};

	static const  ChannelDefinition CHANNEL_DEFINITIONS[] = {
		{ CT_INT, sizeof(int) },
		{ CT_FLOAT, sizeof(float) },
		{ CT_V2, sizeof(v2) },
		{ CT_V3, sizeof(v3) },
		{ CT_V4, sizeof(v4) },
		{ CT_RECT, sizeof(Rect) },
		{ CT_COLOR, sizeof(Color) },
		{ CT_ID, sizeof(ID)},
		{ CT_TWN, sizeof(tweening::TweeningType) }
	};

	struct ChannelArray {

		char* data;
		int size;
		int capacity;
		int total_capacity;
		int _sizes[MAX_BLOCKS];
		int _indices[MAX_BLOCKS];
		ChannelType _types[MAX_BLOCKS];
		int _num_blocks;
		int* _sparse;

		ChannelArray();

		~ChannelArray();

		void init(ChannelType* types, int num);

		void init(int* sizes, int num);

		ID add();

		template<class T>
		void set(ID id, int channel, const T& t) {
			T* p = (T*)get_ptr(channel);
			assert(_sparse[id] != -1);
			unsigned short index = _sparse[id & INDEX_MASK];
			p[index] = t;
		}

		void set(ID id, int channel, const Texture& t) {
			Texture* p = (Texture*)get_ptr(channel);
			assert(_sparse[id] != -1);
			unsigned short index = _sparse[id & INDEX_MASK];
			p[index] = t;
		}

		void set(ID id, int channel, const Rect& t) {
			Rect* p = (Rect*)get_ptr(channel);
			assert(_sparse[id] != -1);
			unsigned short index = _sparse[id & INDEX_MASK];
			p[index] = t;
		}

		const bool contains(ID id) const {
			return _sparse[id] != -1;
		}

		void remove(ID id);

		template<class T>
		const T& get(ID id, int channel) const {
			const T* p = get_ptr(channel);
			assert(_sparse[id] != -1);
			unsigned short index = _sparse[id & INDEX_MASK];
			return p[index];
		}

		template<class T>
		T& get(ID id, int channel) {
			T* p = (T*)get_ptr(channel);
			assert(_sparse[id] != -1);
			unsigned short index = _sparse[id & INDEX_MASK];
			return p[index];
		}

		void* get_ptr(int channel);

		void* getPointer(int channel,ChannelType type);

		bool resize(int new_size);

		int find(int data_index) const;

		int find_free() const;

	};

}
