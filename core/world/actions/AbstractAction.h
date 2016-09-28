#pragma once
#include "..\..\math\tweening.h"
#include "..\..\string\StaticHash.h"
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "..\..\lib\BlockArray.h"

namespace ds {

	class AbstractAction {

		public:
			AbstractAction(ChannelArray* array, const char* name) : _array(array), _name(name) {
				_hash = StaticHash(name);
			}
			virtual ~AbstractAction() {}
			virtual void update(float dt,ActionEventBuffer& buffer) = 0;
			void removeByIndex(int i);
			virtual void debug() = 0;
			virtual void debug(ID sid) = 0;
			void setBoundingRect(const Rect& r);
			virtual void allocate(int sz) = 0;
			virtual ActionType getActionType() const = 0;
			void clear();
			void removeByID(ID id);
			bool contains(ID id);
			const char* getName() const {
				return _name;
			}
			StaticHash getHash() const {
				return _hash;
			}
		protected:
			int create(ID id);
			int find(ID id);
			ID swap(int index);
			Rect m_BoundingRect;
			BlockArray _buffer;
			ID* _ids;
			int* _channels;
			ChannelArray* _array;
		private:
			const char* _name;
			StaticHash _hash;
		};

	

}
