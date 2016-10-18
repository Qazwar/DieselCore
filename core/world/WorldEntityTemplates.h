#pragma once
#include "..\lib\collection_types.h"
#include "..\io\DataFile.h"
#include "..\string\StaticHash.h"
#include "World.h"

namespace ds {

	class WorldEntityTemplates : public JSONAssetFile {

		struct SheetEntry {
			StaticHash hash;
			WorldEntity entity;
		};

	public:
		WorldEntityTemplates(const char* fileName);
		~WorldEntityTemplates();
		void add(const char* name, const WorldEntity& entity);
		int findIndex(const char* name) const;
		int findIndex(const StaticHash& sid) const;
		bool loadData(const JSONReader& loader);
		bool reloadData(const JSONReader& loader);
	private:
		Array<SheetEntry> _entries;
	};

}