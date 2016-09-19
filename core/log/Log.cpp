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

#pragma warning(disable: 4996)

struct LogContext {

	bool useConsole;
	bool useFile;
	HANDLE console;
	FILE* file;
	int logTypes;

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

void init_logger(int logTypes, int width, int height) {
	_logContext = new LogContext;
	_logContext->logTypes = logTypes;
	if ((logTypes & LogTypes::LT_CONSOLE) == LogTypes::LT_CONSOLE) {
		_logContext->useConsole = true;
		CONSOLE_SCREEN_BUFFER_INFO coninfo;
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		_logContext->console = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(_logContext->console, &coninfo);
		coninfo.dwSize.Y = height;
		coninfo.dwSize.X = width;
		SetConsoleScreenBufferSize(_logContext->console, coninfo.dwSize);

		SMALL_RECT srctWindow;
		srctWindow = coninfo.srWindow;
		srctWindow.Top = 0;
		srctWindow.Left = 0;
		srctWindow.Right = width - 1;
		srctWindow.Bottom = height - 1;
		SetConsoleWindowInfo(_logContext->console, TRUE, &srctWindow);
		SetConsoleTextAttribute(_logContext->console, FOREGROUND_RED);
		const char* text = "Hello world\n";
		unsigned long res;
		WriteConsole(_logContext->console, text, strlen(text), &res, 0);
		SetConsoleTextAttribute(_logContext->console, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	}
	else {
		_logContext->useConsole = false;
	}
	if ((logTypes & LogTypes::LT_FILE) == LogTypes::LT_FILE) {
		_logContext->file = fopen("log.txt", "w");
		_logContext->useFile = true;
	}
	else {
		_logContext->useFile = false;
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
}

Log::Log(const Log& orig) {}

std::ostringstream& Log::get() {
    os << NowTime();
    os << " : ";
    return os;
}

void Log::log_file_line(const char *file, const unsigned long line, bool isError) {
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

std::ostringstream& Log::get(const char *file, const unsigned long line) {
	log_file_line(file, line, false);
	return os;
}

std::ostringstream& Log::error(const char *file, const unsigned long line) {
	log_file_line(file, line, true);
	_errorFlag = true;
	return os;
}

std::ostringstream& Log::error(const char *file, const unsigned long line,const char* message) {
	log_file_line(file, line,true);
	os << message;
	_errorFlag = true;
	return os;
}

std::ostringstream& Log::error(const char *file, const unsigned long line, char* format, va_list args) {
	log_file_line(file, line, true);
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int written = vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
	os << buffer;
	_errorFlag = true;
	return os;
}

std::ostringstream& Log::error(const char *file, const unsigned long line, char* format, ...) {
	va_list args;
	va_start(args, format);
	error(file, line, format, args);
	va_end(args);
	_errorFlag = true;
	return os;
}

std::ostringstream& Log::error() {
	os << NowTime();
	os << " [ERROR] : ";
	_errorFlag = true;
	return os;
}

Log::~Log() {
    os << std::endl;
	//OutputDebugStringA(os.str().c_str());
	unsigned long res;
	if (_logContext->useConsole) {
		if (_errorFlag) {
			SetConsoleTextAttribute(_logContext->console, FOREGROUND_RED);
		}
		else {
			SetConsoleTextAttribute(_logContext->console, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		}
		WriteConsole(_logContext->console, os.str().c_str(), strlen(os.str().c_str()), &res, 0);
	}
	if (_logContext->useFile) {
		fprintf(_logContext->file, "%s", os.str().c_str());
	}
	//handler().write(os.str());
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
