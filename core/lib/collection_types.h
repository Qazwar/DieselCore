#pragma once
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include "..\memory\DefaultAllocator.h"
#include <Vector.h>
#include <Point.h>

namespace ds {

	template<class T,int SIZE>
	class NonGrowingPolicy {
	public:
		NonGrowingPolicy(int max) : _max(max) {}
		bool allowed(int size) {
			if (size >= _max) {
				return false;
			}
			return true;
		}
	private:
		int _max;
	};

	template<class T>
	class EndlessGrowingPolicy {
	public:
		bool allowed(int size) {
			return true;
		}
	};

	template <typename TYPE> void Construct(void* dest) {
		new ((TYPE*)dest) TYPE;
	}

	template <typename TYPE> void Destruct(void* dest) {
		((TYPE*)dest)->~TYPE();
	}

	struct CharBuffer {

		char* data;
		int size;
		int capacity;
		int num;
		Allocator* _allocator;

		CharBuffer(Allocator* allocator = gDefaultMemory);
		CharBuffer(const CharBuffer& b);
		CharBuffer(CharBuffer&& other);
		CharBuffer& operator =(const CharBuffer& b);
		~CharBuffer();

		void* alloc(int sz);
		void resize(int newCap);
		int append(const char* s, int len);
		int append(const char* s);
		int append(char s);
		const char* get(int index) const;

	};

	extern CharBuffer* gStringBuffer;

	class StringStream {

	public:
		StringStream(Allocator* allocator = gDefaultMemory);
		~StringStream();
		void append(const char* s);
		void append(const char* s,int len);
		void append(char s);
		const char* c_str();
		void printf(const char *fmt, ...);

		template <typename T>
		void printf_small(const char *fmt, const T &t) {
			char s[32] = { '\0' };
			_snprintf_s(s, 32, fmt, t);
			append(s);
		}
	private:
		void* alloc(int sz);
		void resize(int newCap);
		char* _data;
		int _size;
		int _capacity;
		Allocator* _allocator;
	};

	inline StringStream& operator<<(StringStream& s, char v) {
		s.append(v);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, const char* v) {
		s.append(v);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, uint32_t v) {
		s.printf_small<uint32_t>("%d", v);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, float v) {
		s.printf_small("%g", v);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, int v) {
		s.printf_small("%d", v);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, double v) {
		s.printf_small("%g", v);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, const v2& v) {
		s.printf("x: %g y: %g", v.x, v.y);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, const v3& v) {
		s.printf("x: %g y: %g z: %g", v.x, v.y, v.z);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, const v4& v) {
		s.printf("x: %g y: %g z: %g w: %g", v.x, v.y, v.z, v.w);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, const Color& v) {
		s.printf("x: %g y: %g z: %g w: %g", v.r, v.g, v.b, v.a);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, const p2i& v) {
		s.printf("x: %d y: %d", v.x, v.y);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, const std::string& v) {
		s.append(v.c_str());
		return s;
	}

	inline StringStream& operator<<(StringStream& s, unsigned long v) {
		s.printf("%lu", v);
		return s;
	}

	inline StringStream& operator<<(StringStream& s, const Rect& r) {
		s.printf("top: %d left: %d width: %d height: %d", r.top,r.left,r.width(),r.height());
		return s;
	}

	inline StringStream& operator<<(StringStream& s, const Texture& t) {
		s.printf("top: %d left: %d dim.x: %d dim.y: %d", t.rect.top,t.rect.left,t.dim.x,t.dim.y);
		return s;
	}

	template<class T>
	class Array {

	public:
		Array(Allocator* allocator = gDefaultMemory) : _allocator(allocator), _size(0), _capacity(0), _data(0) {
			_typeSize = sizeof(T);
			_constructor = !__has_trivial_constructor(T);
			_destructor = !__has_trivial_destructor(T);
		}

		explicit Array(uint32_t size, Allocator* allocator = gDefaultMemory) : _allocator(allocator), _size(size), _capacity(size) {
			_typeSize = sizeof(T);
			_constructor = !__has_trivial_constructor(T);
			_destructor = !__has_trivial_destructor(T);
			//_data = (uint8_t*)_allocator->allocate(size * _typeSize);
			_data = (uint8_t*)ALLOC(size * _typeSize);
			uint8_t* ptr = _data;
			if (_constructor) {
				for (uint32_t i = 0; i < _size; ++i) {
					Construct<T>(ptr);
					ptr += _typeSize;
				}
			}
		}

		~Array() {
			if (_data != 0) {
				clear();
				//_allocator->deallocate(_data);
				DEALLOC(_data);
			}
		}



		class iterator {
		public:
			typedef iterator self_type;
			typedef T value_type;
			typedef T& reference;
			typedef T* pointer;
			typedef int difference_type;
			iterator(pointer ptr) : ptr_(ptr), _index(0) { }
			iterator(pointer ptr,int idx) : ptr_(ptr), _index(idx) {}
			self_type operator++() { self_type i = *this; ptr_++; ++_index; return i; }
			self_type operator++(int junk) { ptr_++; ++_index; return *this; }
			self_type operator+(int junk) { ptr_ += junk; _index += junk; return *this; }
			reference operator*() { return *ptr_; }
			pointer operator->() { return ptr_; }
			pointer ptr() { return ptr_; }
			bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
			bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
			uint32_t index() { return _index; }
		private:
			uint32_t _index;
			pointer ptr_;
		};

		Array& operator=(const Array &other) {
			if (_size > 0) {
				resize(other._size);
				_size = other._size;
				_capacity = other._capacity;
				//_data = (uint8_t*)_allocator->allocate(_size * _typeSize);
				_data = (uint8_t*)ALLOC(_size * _typeSize);
				memcpy(_data, other._data, sizeof(T) * _size);
			}
			else {
				_size = other._size;
				_capacity = other._capacity;
				//_data = (uint8_t*)_allocator->allocate(_size * _typeSize);
				_data = (uint8_t*)ALLOC(_size * _typeSize);
				memcpy(_data, other._data, sizeof(T) * _size);
			}
			return *this;
		}

		iterator begin() {
			return iterator(_items);
		}

		iterator end() {
			return iterator(_items + _size);
		}

		void remove(uint32_t index) {
			assert(index < _size);
			uint8_t* ptr = _data + _typeSize * index;
			if (_destructor) {
				Destruct<T>(ptr);
			}
			if (_size - index - 1 > 0) {
				uint32_t d = _size - index - 1;
				uint8_t* old = ptr + _typeSize;
				memcpy(ptr, old, d * _typeSize);
			}
			--_size;
		}

		iterator remove(iterator it) {
			if (_size == 0) {
				return end();
			}
			assert(it.index() < _size);
			T* ptr = it.ptr();
			if (_destructor) {
				Destruct<T>(ptr);
			}
			_items[it.index()] = _items[_size - 1];
			--_size;
			ptr = &_items[it.index()];
			if (_size == 0) {
				return end();
			}
			return iterator(ptr,it.index());
		}

		int remove_all(const T& t) {
			int cnt = 0;
			iterator it = begin();
			while (it != end()) {
				int d = end().ptr() - it.ptr();
				if ((*it) == t) {
					it = remove(it);
					++cnt;
				}
				else {
					++it;
				}
			}
			return cnt;
		}

		bool contains(const T& t) const {
			for (uint32_t i = 0; i < _size; ++i) {
				if (_items[i] == t) {
					return true;
				}
			}
			return false;
		}

		uint32_t size() const {
			return _size;
		}

		uint32_t capacity() const {
			return _capacity;
		}

		void destroy_all() {
			iterator it = begin();
			while (it != end()) {
				delete (*it);
				++it;
			}
			clear();
			DEALLOC(_data);
			_data = 0;
		}

		void clear() {
			if (_destructor) {
				T* ptr = _items;
				for (uint32_t i = 0; i < _size; ++i) {
					Destruct<T>(ptr);
					++ptr;
				}
			}
			_size = 0;
		}

		T& operator[](uint32_t i) {
			assert(i < _size);
			return _items[i];
		}

		const T& operator[](uint32_t i) const {
			assert(i < _size);
			return _items[i];
		}

		T& at(uint32_t i) {
			assert(i < _size);
			return _items[i];
		}

		const T& at(uint32_t i) const {
			assert(i < _size);
			return _items[i];
		}

		void push_back(const T& t) {
			if (_size + 1 > _capacity) {
				grow(_capacity * 2 + 8);
			}
			_items[_size++] = t;
		}

		void push_back(T&& t) {
			if (_size + 1 > _capacity) {
				grow(_capacity * 2 + 8);
			}
			_items[_size++] = t;
		}

		iterator insert(iterator position, const T& t) {
			return insert(position, 1, t);
		}

		iterator insert(iterator position, uint32_t n, const T& t) {
			assert(position.index() < _size);
			if (_size + 1 > _capacity) {
				grow(_capacity * 2 + 8);
			}			
			int delta = _size - position.index();
			if (delta > 0) {
				uint8_t* ptr = _data + position.index() * _typeSize;
				uint8_t* next = ptr + _typeSize;
				memcpy(next, ptr, delta * _typeSize);
			}
			++_size;
			_items[position.index()] = t;
			return iterator(_items + position.index());
		}

		void pop_back() {
			if (_size > 0) {
				if (_destructor) {
					uint8_t* ptr = _data + (_size - 1) * _typeSize;
					Destruct<T>(ptr);
				}
				--_size;
			}
		}

		const T* data() const {
			return _items;
		}

		T* data() {
			return _items;
		}

		const T& front() const {
			assert(_size > 0);
			return _items[0];
		}

		T& front() {
			assert(_size > 0);
			return _items[0];
		}

		const T& back() const {
			assert(_size > 0);
			return _items[_size - 1];
		}

		T& back() {
			assert(_size > 0);
			return _items[_size - 1];
		}
		
		bool empty() const {
			return _size == 0;
		}

		void reserve(uint32_t newCapacity) {
			grow(newCapacity);
		}

		void resize(uint32_t newCapacity) {
			if (newCapacity > _capacity) {
				grow(newCapacity);
			}
			else if (newCapacity < _capacity) {
				shrink(newCapacity);
			}
		}
		
	private:
		void shrink(uint32_t newCapacity) {
			if (newCapacity < _capacity) {
				if (_size > newCapacity) {
					uint8_t* ptr = _data + newCapacity * _typeSize;
					if (_destructor) {
						for (uint32_t i = newCapacity; i < _size; ++i) {
							Destruct<T>(ptr);
							ptr += _typeSize;
						}
					}
					_size = newCapacity;
				}
				//uint8_t* newItems = (uint8_t*)malloc(newCapacity * _typeSize);
				//uint8_t* newItems = (uint8_t*)_allocator->allocate(newCapacity * _typeSize);
				uint8_t* newItems = (uint8_t*)ALLOC(newCapacity * _typeSize);
				if (_data != 0) {
					memcpy(newItems, _data, newCapacity * _typeSize);
				}
				//free(_data);
				//_allocator->deallocate(_data);
				DEALLOC(_data);
				_data = newItems;
				_capacity = newCapacity;
			}
		}

		void grow(uint32_t newCapacity) {
			if (newCapacity > _capacity) {
				//uint8_t* newItems = (uint8_t*)malloc(newCapacity * _typeSize);
				//uint8_t* newItems = (uint8_t*)_allocator->allocate(newCapacity * _typeSize);
				uint8_t* newItems = (uint8_t*)ALLOC(newCapacity * _typeSize);
				if (_data != 0) {
					memcpy(newItems, _data, _size * _typeSize);
				}
				uint8_t* ptr = newItems + _size * _typeSize;
				if (_constructor) {
					for (uint32_t i = _size; i < newCapacity; ++i) {
						Construct<T>(ptr);
						ptr += _typeSize;
					}
				}
				if (_data != 0) {
					//free(_data);
					//_allocator->deallocate(_data);
					DEALLOC(_data);
				}
				_data = newItems;
				_capacity = newCapacity;
			}
		}

		Array(const Array &other) {
			_typeSize = sizeof(T);
			_size = other._size;
			_capacity = other._capacity;
			//_data = (uint8_t*)malloc(_size * _typeSize);
			//_data = (uint8_t*)_allocator->allocate(_size * _typeSize);
			_data = (uint8_t*)ALLOC(_size * _typeSize);
			memccpy(_data, other._data, sizeof(T) * _size);
		}

		Allocator* _allocator;
		bool _constructor;
		bool _destructor;
		uint32_t _typeSize;
		uint32_t _size;
		uint32_t _capacity;
		union {
			uint8_t* _data;
			T* _items;
		};
	};

	// ------------------------------------------------------------
	// BaseStack
	// ------------------------------------------------------------
	template<class T>
	class BaseStack {

	public:
		BaseStack(Allocator* allocator = gDefaultMemory) : _allocator(allocator), _size(0), _capacity(0), _data(0) {
			_typeSize = sizeof(T);
			_constructor = !__has_trivial_constructor(T);
			_destructor = !__has_trivial_destructor(T);
		}

		virtual ~BaseStack() {
			if (_data != 0) {
				clear();
				DEALLOC(_data);
			}
		}

		virtual bool allowed(int size) = 0;

		void push(const T& t) {
			if (allowed(_size)) {
				if (_size + 1 > _capacity) {
					grow(_capacity * 2 + 8);
				}
				_items[_size++] = t;
			}
		}

		void pop() {
			if (_size > 0) {
				uint8_t* ptr = _data + (_size - 1) * _typeSize;
				Destruct<T>(ptr);
				--_size;
			}
		}

		const T& top() const {
			return _items[_size - 1];
		}

		T& top() {
			return _items[_size - 1];
		}

		const T& at(int index) const {
			return _items[_size - index - 1];
		}

		T& at(int index) {
			return _items[_size - index - 1];
		}

		uint32_t size() const {
			return _size;
		}

		bool empty() const {
			return _size == 0;
		}

		void clear() {
			if (_destructor) {
				uint8_t* ptr = _data;
				for (uint32_t i = 0; i < _size; ++i) {
					Destruct<T>(ptr);
					ptr += _typeSize;
				}
			}
			_size = 0;
		}
	private:
		void grow(uint32_t newCapacity) {
			if (newCapacity > _capacity) {
				uint8_t* newItems = (uint8_t*)ALLOC(newCapacity * _typeSize);
				if (_data != 0) {
					memcpy(newItems, _data, _size * _typeSize);
				}
				uint8_t* ptr = newItems + _size * _typeSize;
				if (_data != 0) {
					DEALLOC(_data);
				}
				_data = newItems;
				_capacity = newCapacity;
			}
		}
		Allocator* _allocator;
		uint32_t _size;
		uint32_t _capacity;
		bool _constructor;
		bool _destructor;
		uint32_t _typeSize;
		union {
			uint8_t* _data;
			T* _items;
		};

	};

	// ------------------------------------------------------------
	// Stack
	// ------------------------------------------------------------
	template<class T>
	class Stack : public BaseStack<T> {

	public:
		Stack(Allocator* allocator = gDefaultMemory) : BaseStack(allocator) {}
		virtual ~Stack() {}
		bool allowed(int size) {
			return true;
		}
	};

	// ------------------------------------------------------------
	// FixedSizeStack
	// ------------------------------------------------------------
	template<class T>
	class FixedSizeStack : public BaseStack<T> {

	public:
		FixedSizeStack(int max,Allocator* allocator = gDefaultMemory) : BaseStack(allocator) , _max(max) {}
		virtual ~FixedSizeStack() {}
		bool allowed(int size) {
			if (size >= _max) {
				return false;
			}
			return true;
		}
	private:
		int _max;
	};

}