#pragma once

namespace ds {

	class StateObject {

	public:
		StateObject() : _active(false) {}
		StateObject(bool state) : _active(state) {}
		virtual ~StateObject() {}
		bool isActive() const {
			return _active;
		}
		virtual void onActivation() {}
		virtual void onDeactivation() {}
		void activate() {
			_active = true;
			onActivation();
		}
		void deactivate() {
			_active = false;
			onDeactivation();
		}
	private:
		bool _active;
	};

}
