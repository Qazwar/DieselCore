#pragma once
#include <Windows.h>
#include <stdint.h>

namespace ds {

	struct KeyStates {
		uint8_t ascii;
		bool onChar;
		WPARAM keyPressed;
		bool keyDown;
		WPARAM keyReleased;
		bool keyUp;
	};

	struct ButtonState {
		int button;
		int x;
		int y;
		bool down;
		bool processed;
	};

}