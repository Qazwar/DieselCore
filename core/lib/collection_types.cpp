#include "collection_types.h"

namespace ds {

	CharBuffer* gStringBuffer;

	CharBuffer::CharBuffer(Allocator* allocator) : data(nullptr), size(0), capacity(0), num(0), _allocator(allocator) {}

	CharBuffer::CharBuffer(const CharBuffer& b) {
		_allocator = gDefaultMemory;
		int sz = b.size;
		resize(sz);
		memcpy(data, b.data, sz);
	}

	CharBuffer::CharBuffer(CharBuffer&& other) : size(0) , capacity(0) , data(nullptr) {
		_allocator = gDefaultMemory;
		size = other.size;
		data = other.data;
		capacity = other.capacity;
		// reset other
		other.size = 0;
		other.capacity = 0;
		other.data = nullptr;
	}

	CharBuffer& CharBuffer::operator =(const CharBuffer& b) {
		int sz = b.size;
		resize(sz);
		memcpy(data, b.data, sz);
		return *this;
	}

	CharBuffer::~CharBuffer() {
		if (data != nullptr) {
			DEALLOC(data);
		}
	}

	void* CharBuffer::alloc(int sz) {
		if (size + sz > capacity) {
			int d = capacity * 2 + 8;
			if (d < sz) {
				d = sz * 2 + 8;
			}
			resize(d);
		}
		auto res = data + size;
		size += sz;
		int d = sz / 4;
		if (d == 0) {
			d = 1;
		}
		num += d;
		return res;
	}

	void CharBuffer::resize(int newCap) {
		if (newCap > capacity) {
			char* tmp = (char*)ALLOC(newCap);
			if (data != nullptr) {
				memcpy(tmp, data, size);
				DEALLOC(data);
			}
			capacity = newCap;
			data = tmp;
		}
	}

	const char* CharBuffer::get(int index) const {
		return data + index;
	}

	int CharBuffer::append(const char* s, int len) {
		if (size + len + 1 > capacity) {
			resize(capacity + len + 1 + 8);
		}
		const char* t = s;
		int ret = size;
		for (int i = 0; i < len; ++i) {
			data[size++] = *t;
			++t;
		}
		data[size++] = '\0';
		return ret;
	}

	int CharBuffer::append(const char* s) {
		int len = strlen(s);
		return append(s, len);
	}

	int CharBuffer::append(char s) {
		if (size + 1 > capacity) {
			resize(capacity + 9);
		}
		int ret = size;
		data[size++] = s;
		data[size++] = '\0';
		return ret;
	}


	StringStream::StringStream(Allocator* allocator) : _data(0), _size(0), _capacity(0), _allocator(allocator) {}
	/*
	StringStream::StringStream(const StringStream& b) {
		_allocator = gDefaultMemory;
		int sz = b.size;
		resize(sz);
		memcpy(_data, b.data, sz);
	}

	StringStream& StringStream::operator =(const StringStream& b) {
		int sz = b.size;
		resize(sz);
		memcpy(data, b.data, sz);
		return *this;
	}
	*/
	StringStream::~StringStream() {
		if (_data != 0) {
			DEALLOC(_data);
		}
	}

	void* StringStream::alloc(int sz) {
		if (_size + sz > _capacity) {
			int d = _capacity * 2 + 8;
			if (d < sz) {
				d = sz * 2 + 8;
			}
			resize(d);
		}
		auto res = _data + _size;
		_size += sz;		
		return res;
	}

	void StringStream::resize(int newCap) {
		if (newCap > _capacity) {
			char* tmp = (char*)ALLOC(newCap);
			if (_data != 0) {
				memcpy(tmp, _data, _size);
				DEALLOC(_data);
			}
			_capacity = newCap;
			_data = tmp;
		}
	}

	void StringStream::append(const char* s, int len) {
		if (_size + len + 1 > _capacity) {
			resize(_capacity + len + 1 + 8);
		}
		const char* t = s;
		for (int i = 0; i < len; ++i) {
			_data[_size++] = *t;
			++t;
		}		
	}

	void StringStream::append(const char* s) {
		int len = strlen(s);
		append(s, len);
	}

	void StringStream::append(char s) {
		if (_size + 1 > _capacity) {
			resize(_capacity + 9);
		}
		_data[_size++] = s;
	}

	const char* StringStream::c_str() {
		if (_data[_size - 1] != '\0') {
			append('\0');
		}
		return _data;
	}

	void StringStream::printf(const char *fmt, ...) {
		va_list args;
		va_start(args, fmt);
		int n = vsnprintf(NULL, 0, fmt, args);
		va_end(args);
		uint32_t end = _size;
		resize(end + n + 1);
		va_start(args, fmt);
		vsnprintf(_data + end, n + 1, fmt, args);
		va_end(args);
		_size = end + n;
	}

}