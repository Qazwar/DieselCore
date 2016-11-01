#pragma once
#define PROFILING TRUE
#include <Vector.h>
#include <windows.h>
#include <chrono>
#include "..\log\Log.h"
#include "..\io\ReportWriter.h"

/*
namespace ds {

	class ReportWriter;

}
*/
class StopWatch {

public:	
	StopWatch();
	explicit StopWatch(const char* name);
	~StopWatch();
	void start();
	void end();
	double elapsed();
	double elapsedMS();
private:
	std::chrono::steady_clock::time_point _start, _end;
	char _name[32];
	bool _running;
};

#define TIMER(name) StopWatch s(name); s.start();

// -------------------------------------------------------
// Profile data
// -------------------------------------------------------
struct ProfileSnapshot {

	char name[32];
	int level;
	int invokeCounter;
	float totalTime;

};

namespace perf {

	void init();

	void reset();

	void finalize();

	int start(const char* name);

	void end(int index);

	void shutdown();

	void debug();

	void save(const ds::ReportWriter& writer);

	void tickFPS(float dt);

	void incFrame();
	//void showDialog(v2* position);

	//int get_snapshot(ProfileSnapshot* items, int max);

	float get_current_total_time();

	int startTimer(const char* name);

	void endTimer(int idx);

	void addTimerValue(const char* name, float value);

	int getTimerValues(const char* name,float* values, int max);
}

class ZoneTracker {

public:
	ZoneTracker(const char* name) {
		_index = perf::start(name);
	}
	~ZoneTracker() {
		perf::end(_index);
	}
private:
	int _index;
};

class TimeTracker {

public:
	TimeTracker(const char* name) {
		_index = perf::startTimer(name);
	}
	~TimeTracker() {
		perf::endTimer(_index);
	}
private:
	int _index;
};