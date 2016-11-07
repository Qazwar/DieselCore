#pragma once
#include "..\math\vector.h"
#include "..\math\math_types.h"
#include "..\graphics\Color.h"
#include "..\lib\collection_types.h"
#include "..\io\DataFile.h"
#include "..\string\StaticHash.h"

namespace ds {

	enum SettingsType {
		ST_FLOAT,
		ST_RECT,
		ST_INT,
		ST_PATH,
		ST_COLOR,
		ST_VEC2,
		ST_VEC3,
		ST_NONE
	};

	// -------------------------------------------------------
	// settings item
	// -------------------------------------------------------
	struct SettingsItem {
		const char* name;
		StaticHash hash;
		SettingsType type;
		union {
			int* iPtr;
			float* fPtr;
			v2* v2Ptr;
			v3* v3Ptr;
			Color* cPtr;
			Rect* rPtr;
			V3Path* pPtr;
		} ptr;
	};
	
	// -------------------------------------------------------
	// dynamic settings
	// -------------------------------------------------------
	struct DynamicGameSettings : public FlatJSONAssetFile {

		DynamicGameSettings(const char* fileName);
		~DynamicGameSettings();

		void add(const char* name, float* value, float defaultValue = 0.0f);
		bool set(const char* name, float value);

		void add(const char* name, int* value, int defaultValue);
		bool set(const char* name, int value);

		void add(const char* name, v2* value, const v2& defaultValue);

		//void addRect(const char* name, Rect* value, const Rect& defaultValue);
		//bool setRect(const char* name, const Rect& value);

		void add(const char* name, Color* value, const Color& defaultValue);
		//bool setColor(const char* name, const Color& value);

		void add(const char* name, V3Path* value);
		bool set(const char* name, const V3Path& value);
		
		bool reloadData(const FlatJSONReader& loader);
		bool loadData(const FlatJSONReader& loader);

		//int _state;
		//int _offset;
		//Array<SettingsItem> _items;
		//gui::ComponentModel<SettingsItem> _model;

		Array<SettingsItem> items;
	};

}