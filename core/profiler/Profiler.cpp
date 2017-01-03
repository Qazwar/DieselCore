#include "Profiler.h"
#include "..\log\Log.h"
#include "..\string\StringUtils.h"
//#include "..\ui\IMGUI.h"
//#include "..\DialogResources.h"
#include <algorithm>
#include <functional>
#include "..\string\StaticHash.h"

namespace timer {

	struct BasicTimingInfo {
		LARGE_INTEGER frequency;
		LONGLONG overhead;
	};

	static BasicTimingInfo* basicTimingInfo = 0;

	void init_timing() {
		basicTimingInfo = new BasicTimingInfo;
		QueryPerformanceFrequency(&basicTimingInfo->frequency);
		LARGE_INTEGER start;
		LARGE_INTEGER stop;
		QueryPerformanceCounter(&start);
		QueryPerformanceCounter(&stop);
		basicTimingInfo->overhead = stop.QuadPart - start.QuadPart;
	}

	void shutdown_timing() {
		if (basicTimingInfo != nullptr) {
			delete basicTimingInfo;
		}
	}

}

StopWatch::StopWatch() {
	_running = false;
	_elapsed = 0.0f;
	sprintf_s(_name, 32, "StopWatch");
}

StopWatch::StopWatch(const char* name) {
	_running = false;
	_elapsed = 0.0f;
	sprintf_s(_name, 32, "%s", name);
}

StopWatch::~StopWatch() {
	if (_running) {
		end();
		LOG << _name << " - elapsed: " << elapsed();
	}
}

void StopWatch::start() {
	QueryPerformanceCounter(&_start);
	_running = true;
}

//double StopWatch::LIToSecs(LARGE_INTEGER & L) {
	//return ((double)L.QuadPart * 1000.0 / (double)_frequency.QuadPart);
//}

void StopWatch::end() {
	_running = false;
	QueryPerformanceCounter(&_end);
	_elapsed = (_end.QuadPart - _start.QuadPart - timer::basicTimingInfo->overhead) * 1000.0 / timer::basicTimingInfo->frequency.QuadPart;
}

double StopWatch::elapsed() {
	return _elapsed;
}

double StopWatch::elapsedMS() {
	return _elapsed;
}

namespace perf {

	const int MAX_TIME_TRACKER_VALUES = 16;

	

	// -----------------------------------------------------------
	// time tracker array
	// -----------------------------------------------------------
	struct TimeTrackerArray {
		char name[20];
		StaticHash hash;
		float values[MAX_TIME_TRACKER_VALUES];
		int index;
		int size;
		LARGE_INTEGER started;
	};

	// -----------------------------------------------------------
	// zone tracker event
	// -----------------------------------------------------------
	struct ZoneTrackerEvent {
		StaticHash hash;
		int parent;
		LARGE_INTEGER started;
		float duration;
		int name_index;
		int ident;
	};

	// -----------------------------------------------------------
	// Zone tracker context
	// -----------------------------------------------------------
	struct ZoneTrackerContext {
		LARGE_INTEGER frequency;
		ds::CharBuffer names;
		ds::Array<StaticHash> hashes;
		int current_parent;
		int root_event;
		ds::Array<ZoneTrackerEvent> events;
		int ident;
		int frames;
		float fpsTimer;
		int fps;
		LONGLONG overhead;
		ds::Array<TimeTrackerArray> timeTrackers;
	};

	static ZoneTrackerContext* zoneTrackerCtx = 0;

	double LIToSecs(LARGE_INTEGER & L) {
		return (L.QuadPart - zoneTrackerCtx->overhead) * 1000.0 / zoneTrackerCtx->frequency.QuadPart;
	}

	// -----------------------------------------------------------
	// init
	// -----------------------------------------------------------
	void init() {
		if (zoneTrackerCtx == 0) {
			zoneTrackerCtx = new ZoneTrackerContext;
			QueryPerformanceFrequency(&zoneTrackerCtx->frequency);
			LARGE_INTEGER start;
			LARGE_INTEGER stop;
			QueryPerformanceCounter(&start);
			QueryPerformanceCounter(&stop);
			zoneTrackerCtx->overhead = stop.QuadPart - start.QuadPart;
		}
		zoneTrackerCtx->frames = 0;
		zoneTrackerCtx->fpsTimer = 0.0f;
		zoneTrackerCtx->fps = 0;
	}

	// -----------------------------------------------------------
	// reset
	// -----------------------------------------------------------
	void reset() {
		zoneTrackerCtx->names.size = 0;
		zoneTrackerCtx->events.clear();
		zoneTrackerCtx->ident = 0;
		// create root event
		zoneTrackerCtx->current_parent = -1;
		zoneTrackerCtx->root_event = start("ROOT");		
		zoneTrackerCtx->current_parent = zoneTrackerCtx->root_event;
	}

	// -----------------------------------------------------------
	// finalize
	// -----------------------------------------------------------
	void finalize() {
		end(zoneTrackerCtx->root_event);
	}

	// -----------------------------------------------------------
	// debug
	// -----------------------------------------------------------
	void debug() {
		LOG << "------------------------------------------------------------";
		LOG << " Percent | Accu       | Name";
		LOG << "------------------------------------------------------------";
		float norm = zoneTrackerCtx->events[0].duration;
		char buffer[256];
		char p[10];
		std::string line;
		for (uint32_t i = 0; i < zoneTrackerCtx->events.size(); ++i) {
			const ZoneTrackerEvent& event = zoneTrackerCtx->events[i];
			int ident = event.ident * 2;
			float per = event.duration / norm * 100.0f;
			ds::string::formatPercentage(per, p);
			sprintf(buffer, "%s  | %3.8f | ", p, event.duration);
			line = buffer;
			for (int j = 0; j < ident; ++j) {
				line += " ";
			}
			const char* n = zoneTrackerCtx->names.data + zoneTrackerCtx->events[i].name_index;
			LOG << line << " " << n;
		}
		LOG << "------------------------------------------------------------";

	}

	// -----------------------------------------------------------
	// tick FPS
	// -----------------------------------------------------------
	void tickFPS(float dt) {
		zoneTrackerCtx->fpsTimer += dt;
		if (zoneTrackerCtx->fpsTimer >= 1.0f) {
			zoneTrackerCtx->fpsTimer -= 1.0f;
			zoneTrackerCtx->fps = zoneTrackerCtx->frames;
			zoneTrackerCtx->frames = 0;
		}
	}

	// -----------------------------------------------------------
	// increment frame
	// -----------------------------------------------------------
	void incFrame() {
		++zoneTrackerCtx->frames;
	}

	// -----------------------------------------------------------
	// find by hash
	// -----------------------------------------------------------
	int findHash(const StaticHash& hash) {
		for (uint32_t i = 0; i < zoneTrackerCtx->events.size(); ++i) {
			if (zoneTrackerCtx->events[i].hash == hash) {
				return i;
			}
		}
		return -1;
	}

	// -----------------------------------------------------------
	// start zone tracking
	// -----------------------------------------------------------
	int start(const char* name) {
		// create event
		ZoneTrackerEvent event;
		event.parent = zoneTrackerCtx->current_parent;
		QueryPerformanceCounter(&event.started);
		event.ident = zoneTrackerCtx->ident++;
		event.hash = StaticHash(name);
		event.duration = -1.0f;
		int idx = findHash(event.hash);
		if (idx == -1) {
			event.name_index = zoneTrackerCtx->names.size;
			int l = strlen(name);
			if (zoneTrackerCtx->names.size + l > zoneTrackerCtx->names.capacity) {
				zoneTrackerCtx->names.resize(zoneTrackerCtx->names.capacity + 256);
			}
			zoneTrackerCtx->names.append(name, l);
		}
		else {
			event.name_index = zoneTrackerCtx->events[idx].name_index;
		}
		int eventIndex = zoneTrackerCtx->events.size();
		zoneTrackerCtx->events.push_back(event);
		zoneTrackerCtx->current_parent = eventIndex;
		return eventIndex;
	}

	// -----------------------------------------------------------
	// end zone tracking
	// -----------------------------------------------------------
	void end(int index) {

		ZoneTrackerEvent& event = zoneTrackerCtx->events[index];
		LARGE_INTEGER EndingTime;
		QueryPerformanceCounter(&EndingTime);
		LARGE_INTEGER time;
		time.QuadPart = EndingTime.QuadPart - event.started.QuadPart;
		event.duration = LIToSecs(time);
		if (zoneTrackerCtx->events[zoneTrackerCtx->current_parent].parent != -1) {
			zoneTrackerCtx->current_parent = zoneTrackerCtx->events[zoneTrackerCtx->current_parent].parent;
		}
		--zoneTrackerCtx->ident;
	}

	// -----------------------------------------------------------
	// shutdown
	// -----------------------------------------------------------
	void shutdown() {
		delete zoneTrackerCtx;
	}

	// -----------------------------------------------------------
	// Call aggregator
	// -----------------------------------------------------------
	struct CallAggregator {

		StaticHash hash;
		int calls;
		float total;
		int name_index;

		bool operator > (const CallAggregator& other) const {
			return (total > other.total);
		}
	};
	
	// -----------------------------------------------------------
	// save performance report
	// -----------------------------------------------------------
	void save(const ds::ReportWriter& writer) {
		char buffer[256];
		sprintf_s(buffer, 256, "Perf - Profiling (%d FPS) events: %d", zoneTrackerCtx->fps, zoneTrackerCtx->events.size());
		writer.startBox(buffer);
		const char* HEADERS[] = { "Percent", "Accu", "Name" };
		writer.startTable(HEADERS, 3);
		char p[10];
		float norm = zoneTrackerCtx->events[0].duration;
		float total = 0.0f;
		for (uint32_t i = 0; i < zoneTrackerCtx->events.size(); ++i) {
			writer.startRow();
			const ZoneTrackerEvent& event = zoneTrackerCtx->events[i];		
			if (i > 0) {
				total += event.duration;
			}
			int ident = event.ident * 2;
			float per = event.duration / norm * 100.0f;
			ds::string::formatPercentage(per, p);
			writer.addCell(p);
			writer.addCell(event.duration);
			const char* n = zoneTrackerCtx->names.data + zoneTrackerCtx->events[i].name_index;
			//LOG << n << " = " << event.duration << " = " << total;
			writer.addCell(event.ident * 2, n);
			writer.endRow();
		}
		writer.startRow();
		writer.addCell("");
		writer.addCell(total);
		writer.addCell("Total");
		writer.endRow();
		writer.endTable();
		writer.endBox();

		std::vector<CallAggregator> calls;
		for (uint32_t i = 0; i < zoneTrackerCtx->events.size(); ++i) {
			const ZoneTrackerEvent& event = zoneTrackerCtx->events[i];
			int idx = -1;
			for (size_t j = 0; j < calls.size(); ++j) {
				if (calls[j].hash == event.hash) {
					idx = j;
				}
			}
			if (idx == -1) {
				CallAggregator ac;
				ac.calls = 1;
				ac.hash = event.hash;
				ac.total = event.duration;
				ac.name_index = event.name_index;
				calls.push_back(ac);
			}
			else {
				CallAggregator& ac = calls[idx];
				++ac.calls;
				ac.total += event.duration;
			}
		}
		std::sort(calls.begin(), calls.end(),std::greater<CallAggregator>());
		
		writer.startBox("Perf - Top calls");
		const char* TC_HEADERS[] = { "Calls", "Total", "Name" };
		writer.startTable(TC_HEADERS, 3);
		for (uint32_t i = 0; i < calls.size(); ++i) {
			const CallAggregator& ac = calls[i];
			writer.startRow();
			writer.addCell(ac.calls);
			writer.addCell(ac.total);
			const char* n = zoneTrackerCtx->names.data + ac.name_index;
			writer.addCell(n);
			writer.endRow();
		}
		writer.endTable();
		writer.endBox();

		for (uint32_t i = 0; i < zoneTrackerCtx->timeTrackers.size(); ++i) {
			const TimeTrackerArray& tta = zoneTrackerCtx->timeTrackers[i];
			sprintf(buffer, "TimeTracker - %s", tta.name);
			writer.startBox(buffer);
			const char* TTA_HEADERS[] = { "Index", "Value"};
			writer.startTable(TTA_HEADERS, 2);
			int s = tta.index;
			int m = tta.size;
			for ( int j = 0; j < m; ++j) {
				writer.startRow();
				writer.addCell(s);
				writer.addCell(tta.values[s]);
				writer.endRow();
				++s;
				if (s >= tta.size) {
					s = 0;
				}
			}
			writer.endTable();
			writer.endBox();
		}

	}
	
	// -----------------------------------------------------------
	// get current total time
	// -----------------------------------------------------------
	float get_current_total_time() {
		if (zoneTrackerCtx->events.size() > 0) {
			return zoneTrackerCtx->events[0].duration;
		}
		else {
			return 0.0f;
		}
	}

	// -----------------------------------------------------------
	// find time tracker
	// -----------------------------------------------------------
	int findTimeTracker(const char* name) {
		StaticHash hash = StaticHash(name);
		for (uint32_t i = 0; i < zoneTrackerCtx->timeTrackers.size(); ++i) {
			const TimeTrackerArray& tta = zoneTrackerCtx->timeTrackers[i];
			if (hash == tta.hash) {
				return i;
			}
		}
		return -1;
	}

	// -----------------------------------------------------------
	// start time tracker
	// -----------------------------------------------------------
	int createTimer(const char* name) {
		TimeTrackerArray tta;
		QueryPerformanceCounter(&tta.started);
		tta.hash = StaticHash(name);
		tta.size = 0;
		tta.index = 0;
		strcpy(tta.name, name);
		for (int i = 0; i < MAX_TIME_TRACKER_VALUES; ++i) {
			tta.values[i] = 0.0f;
		}
		zoneTrackerCtx->timeTrackers.push_back(tta);
		return zoneTrackerCtx->timeTrackers.size() - 1;
	}

	// -----------------------------------------------------------
	// start time tracker
	// -----------------------------------------------------------
	int startTimer(const char* name) {
		int idx = findTimeTracker(name);
		if (idx != -1) {
			TimeTrackerArray& tta = zoneTrackerCtx->timeTrackers[idx];
			QueryPerformanceCounter(&tta.started);
			return idx;
		}
		else {
			return createTimer(name);			
		}
	}

	// -----------------------------------------------------------
	// end time tracker
	// -----------------------------------------------------------
	void endTimer(int idx) {
		TimeTrackerArray& tta = zoneTrackerCtx->timeTrackers[idx];
		LARGE_INTEGER EndingTime;
		QueryPerformanceCounter(&EndingTime);
		LARGE_INTEGER time;
		time.QuadPart = EndingTime.QuadPart - tta.started.QuadPart;
		tta.values[tta.index++] = LIToSecs(time);
		if (tta.index >= MAX_TIME_TRACKER_VALUES) {
			tta.index = 0;
		}
		if (tta.size < MAX_TIME_TRACKER_VALUES) {
			++tta.size;
		}
	}

	// -----------------------------------------------------------
	// add time tracker value
	// -----------------------------------------------------------
	void addTimerValue(const char* name, float value) {
		int idx = findTimeTracker(name);
		if (idx == -1) {
			idx = createTimer(name);
		}
		TimeTrackerArray& tta = zoneTrackerCtx->timeTrackers[idx];
		tta.values[tta.index++] = value;
		if (tta.index >= MAX_TIME_TRACKER_VALUES) {
			tta.index = 0;
		}
		if (tta.size < MAX_TIME_TRACKER_VALUES) {
			++tta.size;
		}
	}

	// -----------------------------------------------------------
	// get time tracker values
	// -----------------------------------------------------------
	int getTimerValues(const char* name,float* values, int max) {
		int idx = findTimeTracker(name);
		if ( idx != -1 ) {
			const TimeTrackerArray& tta = zoneTrackerCtx->timeTrackers[idx];
			int cnt = 0;
			for (int j = 0; j < tta.size; ++j) {
				if (cnt < max) {
					values[cnt++] = tta.values[j];
				}
			}
			return cnt;
		}
		return 0;
	}
	/*
	void showDialog(v2* position) {
		gui::start(EDITOR_ID, position);
		int state = 1;
		if (gui::begin("Profiler", &state)) {
			char buffer[256];
			float norm = zoneTrackerCtx->events[0].duration;
			for (int i = 0; i < zoneTrackerCtx->events.size(); ++i) {
				const ZoneTrackerEvent& event = zoneTrackerCtx->events[i];
				int ident = pd.level * 2;
				//float per = pd.totalTime / norm * 100.0f;
				sprintf(buffer, "%3.8f - %2.11s", pd.totalTime, pd.name);
				gui::Label(buffer);
			}
		}
		gui::end();
	}

	int get_snapshot(ProfileSnapshot* items, int max) {
		int realMax = ctx.index;
		if (realMax > max) {
			realMax = max;
		}
		float total = 0.0f;
		for (int i = 0; i < realMax; ++i) {
			ProfileData& pd = ctx.data[i];
			ProfileSnapshot& snap = items[i];
			strcpy(snap.name, pd.name);
			snap.level = pd.level;
			snap.invokeCounter = pd.invokeCounter;
			snap.totalTime = pd.totalTime;
			total += pd.totalTime;
		}
		items[0].totalTime = total;
		return realMax;
	}
	*/
}

