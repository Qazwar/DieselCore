/* 
 * File:   Log.cpp
 * Author: mecky
 * 
 * Created on 3. November 2010, 08:05
 */

#include "Log.h"
#include <windows.h>
#include <stdio.h>
#include "..\base\EventStream.h"
#include "..\string\StaticHash.h"
#include "..\io\json.h"
#include <vector>

#pragma warning(disable: 4996)

void reportLastError() {
	char lpMsgBuf[255];
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		255, NULL);

	if (lpMsgBuf != 0) {
		LOG << "ERROR: " << lpMsgBuf;
		//LocalFree(lpMsgBuf);
	}
}

struct LogCategory {
	StaticHash hash;
	LogLevel level;
	//int nameIndex;
};

struct LogLevelDefinition {
	const char* name;
	LogLevel level;
	StaticHash hash;

	LogLevelDefinition(const char* n, LogLevel l) : name(n), level(l) {
		hash = StaticHash(n);
	}
};

static const LogLevelDefinition LOGLEVELS[] = {
	{"TRACE" , LL_TRACE },
	{"DEBUG" , LL_DEBUG },
	{"INFO"  , LL_INFO},
	{"WARN"  , LL_WARN},
	{"ERROR" , LL_ERROR}
};

int find_log_level(const char* string) {
	StaticHash hash = StaticHash(string);
	for (int i = 0; i < 5; ++i) {
		if (LOGLEVELS[i].hash == hash) {
			return i;
		}
	}
	return -1;
}

struct LogContext {

	bool available;
	bool useConsole;
	bool useFile;
	uint32_t consoleWidth;
	uint32_t consoleHeight;
	HANDLE console;
	FILE* file;
	LogLevel rootLevel;
	std::vector<LogCategory> categories; // we cannot use ds::Array here

	LogContext() {
		available = false;
		useConsole = false;
		useFile = false;
		file = 0;
	}

	bool load() {
		available = false;
		ds::JSONReader reader;
		if (reader.parseFile("content\\log.json")) {
			int settings = reader.find_category("settings");
			if (settings != -1) {
				reader.get(settings, "console", &useConsole);
				reader.get(settings, "file", &useFile);
				reader.get(settings, "console_width", &consoleWidth);
				reader.get(settings, "console_height", &consoleHeight);
				if (useConsole || useFile) {
					available = true;
				}
				const char* str = reader.get_string(settings, "root_level");
				int lvl = find_log_level(str);
				if (lvl != -1) {
					rootLevel = LOGLEVELS[lvl].level;
				}
				else {
					rootLevel = LL_INFO;
				}
			}
			int mainCategory = reader.find_category("categories");
			if (mainCategory != -1) {
				int cats[64];
				int num = reader.get_categories(cats, 64, mainCategory);
				for (int i = 0; i < num; ++i) {
					const char* name = reader.get_string(cats[i], "name");
					const char* lvlName = reader.get_string(cats[i], "level");
					LogCategory cat;
					int lvl = find_log_level(lvlName);
					if (lvl != -1) {
						cat.level = LOGLEVELS[lvl].level;
					}
					else {
						cat.level = LL_ERROR;
					}
					cat.hash = StaticHash(name);
					categories.push_back(cat);
				}
			}
			return true;
		}
		return false;
	}

	LogLevel find(StaticHash category) {
		for (uint32_t i = 0; i < categories.size(); ++i) {
			if (categories[i].hash == category) {
				return categories[i].level;
			}
		}
		return rootLevel;
	}

};

void MyAssert(char* expr_str, bool expr, char* file, int line, char* msg) {
	if (!expr) {
		LOG << "---------------------------------------------------------------";
		LOGE << msg;
		LOG << "---------------------------------------------------------------";
		MessageBoxA(GetDesktopWindow(), msg, "ERROR", NULL);
		abort();
	}
}

void MyAssert_fmt(char* expr_str, bool expr, char* file, int line, char* format, ...) {
	if (!expr) {
		va_list args;
		va_start(args, format);
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		int written = vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
		LOG << "---------------------------------------------------------------";
		LOGE << buffer;
		LOG << "---------------------------------------------------------------";
		MessageBoxA(GetDesktopWindow(), buffer, "ERROR", NULL);
		va_end(args);
		abort();
	}
}

void MyAssert_fmt(char* file, int line, char* format, ...) {
	va_list args;
	va_start(args, format);
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int written = vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
	LOG << "---------------------------------------------------------------";
	LOGE << buffer;
	LOG << "---------------------------------------------------------------";
	MessageBoxA(GetDesktopWindow(), buffer, "ERROR", NULL);
	va_end(args);
	abort();
}


static LogContext* _logContext;

void init_logger() {
	_logContext = new LogContext;
	if (_logContext->load()) {
		if (_logContext->useConsole) {
			_logContext->useConsole = true;
			CONSOLE_SCREEN_BUFFER_INFO coninfo;
			AllocConsole();
			AttachConsole(GetCurrentProcessId());
			_logContext->console = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(_logContext->console, &coninfo);
			coninfo.dwSize.Y = _logContext->consoleHeight;
			coninfo.dwSize.X = _logContext->consoleWidth;
			COORD coord;
			coord.X = _logContext->consoleWidth;
			coord.Y = _logContext->consoleHeight;

			SMALL_RECT srctWindow;
			//srctWindow = coninfo.srWindow;
			srctWindow.Top = 0;
			srctWindow.Left = 0;
			srctWindow.Right = _logContext->consoleWidth - 1;
			srctWindow.Bottom = _logContext->consoleHeight - 1;
			SetConsoleWindowInfo(_logContext->console, TRUE, &srctWindow);

			SetConsoleScreenBufferSize(_logContext->console, coord);

			GetConsoleScreenBufferInfo(_logContext->console, &coninfo);

			HWND consoleWindow = GetConsoleWindow();
			SetWindowPos(consoleWindow, 0, 0, 0, _logContext->consoleWidth, _logContext->consoleHeight, SWP_NOSIZE | SWP_NOZORDER);
			SetConsoleTextAttribute(_logContext->console, FOREGROUND_RED);
			//const char* text = "Hello world\n";
			//unsigned long res;
			//WriteConsole(_logContext->console, text, strlen(text), &res, 0);
			//SetConsoleTextAttribute(_logContext->console, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		}
		if (_logContext->useFile) {
			_logContext->file = fopen("log.txt", "w");
		}
	}
}

void shutdown_logger() {
	if (_logContext->useConsole) {
		FreeConsole();
	}
	if (_logContext->useFile) {
		fclose(_logContext->file);
	}
	delete _logContext;
}

void file_name(const char* file, char* name) {
	char e[16];
	_splitpath(file,0,0,name,e);
}

void ConsoleOutputHandler::write(const char* message) {
	OutputDebugStringA(message);
}

FILE*& FileOutputHandler::Stream() {
    static FILE* pStream = stderr;
    return pStream;
}

void FileOutputHandler::write(const char* message) {
	OutputDebugStringA(message);
	FILE* pStream = Stream();
	if ( pStream ) {
		fprintf(pStream, "%s", message);
		fflush(pStream);
	}
}

Log::Log() {
	_errorFlag = false;
	_level = LL_ERROR;
	_categoryLevel = LL_ERROR;
}

Log::Log(StaticHash category,LogLevel level) {
	_errorFlag = false;
	_level = level;
	_categoryLevel = _logContext->find(category);
}

Log::Log(const Log& orig) {}

bool Log::matches() {
	return (_logContext->available && _level >= _categoryLevel);
}

ds::StringStream& Log::get() {
	if (matches()) {
		os << NowTime();
		os << " : ";
	}
    return os;
}

void Log::log_file_line(const char *file, const unsigned long line, bool isError) {
	if (matches()) {
		char buffer[128];
		NowTime(buffer, 128);
		os << buffer;
		os << " ";
		if (isError) {
			os << " -- ERROR -- ";
			_errorFlag = true;
		}
		file_name(file, buffer);
		os << " [";
		os << buffer;
		os << ":";
		os << line;
		os << "] : ";
	}
}

ds::StringStream& Log::get(const char *file, const unsigned long line) {
	if (matches()) {
		log_file_line(file, line, false);
	}
	return os;
}

ds::StringStream& Log::error(const char *file, const unsigned long line) {
	if (matches()) {
		log_file_line(file, line, true);
	}
	_errorFlag = true;
	return os;
}

ds::StringStream& Log::error(const char *file, const unsigned long line, const char* message) {
	if (matches()) {
		log_file_line(file, line, true);
		os << message;
	}
	_errorFlag = true;
	return os;
}

ds::StringStream& Log::error(const char *file, const unsigned long line, char* format, va_list args) {
	_level = LL_ERROR;
	if (matches()) {
		log_file_line(file, line, true);
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		int written = vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
		os << buffer;
	}
	_errorFlag = true;
	return os;
}

ds::StringStream& Log::error(const char *file, const unsigned long line, char* format, ...) {
	_level = LL_ERROR;
	if (matches()) {
		va_list args;
		va_start(args, format);
		error(file, line, format, args);
		va_end(args);
	}
	_errorFlag = true;
	return os;
}

ds::StringStream& Log::error() {
	_level = LL_ERROR;
	if (matches()) {
		os << NowTime();
		os << " [ERROR] : ";
	}
	_errorFlag = true;
	return os;
}

Log::~Log() {
	if (matches()) {
		os << '\n';
		unsigned long res;
		if (_logContext->useConsole) {
			if (_errorFlag) {
				SetConsoleTextAttribute(_logContext->console, FOREGROUND_RED);
			}
			else {
				SetConsoleTextAttribute(_logContext->console, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
			}
			WriteConsole(_logContext->console, os.c_str(), strlen(os.c_str()), &res, 0);
		}
		if (_logContext->useFile) {
			fprintf(_logContext->file, "%s", os.c_str());
		}
	}
}

LogOutputHandler& Log::handler() {
	static ConsoleOutputHandler coh;
	return coh;	
}

void Log::NowTime(char* ret, int max) {
	char buffer[200];
	if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, "HH':'mm':'ss", buffer, 200) == 0) {
		//return "Error in NowTime()";
	}

	static DWORD first = GetTickCount();
	sprintf_s(ret, max, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);
}

std::string Log::NowTime() {
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
            "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);
    return result;
}
