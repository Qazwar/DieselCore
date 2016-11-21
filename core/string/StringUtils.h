#pragma once
#include <stdint.h>
#include <string>
#include "..\Common.h"
#include "..\graphics\Color.h"
#include "..\math\math.h"
#include "..\lib\collection_types.h"
#include "..\utils\GameTimer.h"

namespace ds {

namespace string {

	//Array<std::string> split(const std::string& str,const char delimiter = ',');

	int count_delimiters(const char* text, const char delimiter = ',');

	int count_delimiters(const char* text, int* positions, int max, const char delimiter = ',');

	int split(const char* text, float* array, int max, const char delimiter = ',');

	void split(const std::string& str,Array<std::string>& list,const char delimiter = ',');

	void trim(std::string& str);

	void formatPercentage(float percentage, char* ret);

	void trim(std::string& str,const std::string& ws);
	
	void formatInt(int value,std::string& str,int length);

	void formatInt(int value, char* str, int length, int offset = 0);

	void formatInt(int value,std::string& str);

	void format_number(int n, char * out, const char delimiter = '.');

	int extract_used_characters(const char* text, char* array, int max);

	void formatTime(int minutes,int seconds,std::string& str);

	void formatTime(const GameTimer& timer, char* str);

	void file_name(const char* file, char* name);

	void formatDateTime(std::string& str);

	bool isDigit(const char c);

	bool isDigit(const char* c);

	bool isNumeric(const char* text);

	bool isWhitespace(const char c);

	float strtof(const char* p,char** endPtr);

	bool isCharacter(const char p);

	int find(char* text, char* pattern);


	template <class T>
	inline std::string to_string (const T& t) {
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	template<class T> 
	T inline from_string(const std::string& s) {
		std::istringstream stream (s);
		T t;
		stream >> t;
		return t;
	}


}

}

