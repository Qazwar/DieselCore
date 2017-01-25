#pragma once
#include <stdint.h>

namespace ds {

	namespace events {

		enum SystemEvents {
			SE_SHUTDOWN_SYSTEM,
			SE_BUTTON_ONE_DOWN,
			SE_BUTTON_ONE_UP
		};

		void init();

		void reset();

		void send(uint32_t type);

		void send(uint32_t type, void* p, size_t size);

		bool get(uint32_t index, void* p);
		
		int getType(uint32_t index);

		bool containsType(uint32_t type);

		uint32_t num();

		void shutdown();

	}
}


