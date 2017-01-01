#pragma once
#include "..\io\json.h"
#include "..\profiler\Profiler.h"
#include "..\lib\collection_types.h"
#include <assert.h>

namespace ds {

	// -----------------------------------------------
	// DataFile
	// -----------------------------------------------
	class DataFile {

	public:
		DataFile(const char* fileName) : _loaded(false) {
			_nameIndex = gStringBuffer->append(fileName);
			_hash = StaticHash(fileName);
		}
		virtual ~DataFile() {}
		const StaticHash& getFileName() const {
			return _hash;
		}
		const char* getPlainFileName() const {
			return gStringBuffer->get(_nameIndex);
		}
		virtual bool load() = 0;
		bool isLoaded() const {
			return _loaded;
		}
	protected:
		bool _loaded;
		StaticHash _hash;
		int _nameIndex;
	};

	// -----------------------------------------------
	// JSON file
	// -----------------------------------------------
	class JSONAssetFile : public DataFile {

	public:
		JSONAssetFile(const char* name) : DataFile(name) {}
		virtual ~JSONAssetFile() {}
		virtual bool loadData(const JSONReader& loader) = 0;
		virtual bool reloadData(const JSONReader& loader) = 0;
		bool load();
	};

	// -----------------------------------------------
	// Flat JSON file
	// -----------------------------------------------
	class FlatJSONAssetFile : public DataFile {

	public:
		FlatJSONAssetFile(const char* name) : DataFile(name) {}
		virtual ~FlatJSONAssetFile() {}
		virtual bool loadData(const FlatJSONReader& loader) = 0;
		virtual bool reloadData(const FlatJSONReader& loader) = 0;
		bool load();
	};

	// -----------------------------------------------
	// Text Asset file
	// -----------------------------------------------
	class TextAssetFile : public DataFile {

	public:
		TextAssetFile(const char* name) : DataFile(name) {}
		virtual ~TextAssetFile() {}
		virtual bool loadData(const char* text) = 0;
		virtual bool reloadData(const char* text) = 0;
		bool load();
	};

}