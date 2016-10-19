#pragma once
#include "..\lib\collection_types.h"
#include "..\io\DataFile.h"
#include "..\string\StaticHash.h"

namespace ds {

	struct WorldEntity {

		v3 position;
		v3 scale;
		v3 rotation;
		Texture texture;
		Color color;
		float timer;
		int type;
		v3 force;
	};

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
		const WorldEntity& getTemplate(int idx) const;
	private:
		Array<SheetEntry> _entries;
	};

}