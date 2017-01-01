#pragma once
#include <sstream>
#include <stdio.h>
#include "..\lib\collection_types.h"
#include "..\string\StaticHash.h"

void reportLastError();

class AssertOutputHandler {

public:
	virtual void write(char* file, int line, char* msg) = 0;
};


void MyAssert(char* expr_str, bool expr, char* file, int line, char* msg);
void MyAssert_fmt(char* expr_str, bool expr, char* file, int line, char* fomat, ...);
void MyAssert_fmt(char* file, int line, char* format, ...);

void init_logger();

void shutdown_logger();

class LogOutputHandler {

public:
	virtual void write(const char* message) = 0;
};

class ConsoleOutputHandler : public LogOutputHandler {

public:
	void write(const char* message);
};

class FileOutputHandler : public LogOutputHandler {

public:
   static FILE*& Stream();
   void write(const char* msg);

};

enum LogLevel {
	LL_TRACE,
	LL_DEBUG,
	LL_INFO,
	LL_WARN,
	LL_ERROR
};
/*
struct LogCategoryDefinition {
	const char* category;
	LogLevel level;
};

struct LogConfiguration {
	bool useConsole;
	int consoleHeight;
	int consoleWidth;
	bool useFile;
	const char* fileName;
	ds::Array<LogCategoryDefinition> definitions;

	LogConfiguration() : useConsole(true), useFile(true), fileName("log.txt"), consoleHeight(80), consoleWidth(400) {
	}

	void addCategory(const char* name, LogLevel lvl) {
		LogCategoryDefinition def;
		def.category = name;
		def.level = lvl;
		definitions.push_back(def);
	}
};
*/
class Log {
    
public:
    Log();    
	Log(StaticHash category, LogLevel level);
    virtual ~Log();
	ds::StringStream& get();
	ds::StringStream& error();
	ds::StringStream& get(const char *file, const unsigned long line);
	ds::StringStream& error(const char *file, const unsigned long line);
	ds::StringStream& error(const char *file, const unsigned long line, const char* message);
	ds::StringStream& error(const char *file, const unsigned long line, char* format, va_list args);
	ds::StringStream& error(const char *file, const unsigned long line, char* format, ...);
	static LogOutputHandler& handler();        
protected:
	ds::StringStream os;
private:
	bool matches();
	void log_file_line(const char *file, const unsigned long line, bool isError);
    std::string NowTime();
	void NowTime(char* ret, int max);
    Log(const Log&);
    Log& operator =(const Log&);	
	bool _errorFlag;
	LogLevel _level;
	LogLevel _categoryLevel;
};

#define LG \
	Log().get()

#define LOG Log().get(__FILE__,__LINE__)
#define LOGC(CATEGORY) Log(SID(CATEGORY),LL_DEBUG).get(__FILE__,__LINE__)
#define LOGT(CATEGORY) Log(SID(CATEGORY),LL_TRACE).get(__FILE__,__LINE__)
#define LOGD(CATEGORY) Log(SID(CATEGORY),LL_DEBUG).get(__FILE__,__LINE__)
#define LOGI(CATEGORY) Log(SID(CATEGORY),LL_INFO).get(__FILE__,__LINE__)
#define LOGW(CATEGORY) Log(SID(CATEGORY),LL_WARN).get(__FILE__,__LINE__)
#define LOGE Log().error(__FILE__,__LINE__)

#define ELOG(M) \
	do {\
		Log().error(__FILE__,__LINE__,M); \
	} while (0)



