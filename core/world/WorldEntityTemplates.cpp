#include "WorldEntityTemplates.h"
#include "..\log\Log.h"
#include "..\string\StringUtils.h"
#include "..\base\Assert.h"

namespace ds {

	WorldEntityTemplates::WorldEntityTemplates(const char* fileName) : JSONAssetFile(fileName) {
	}

	WorldEntityTemplates::~WorldEntityTemplates() {
	}

	void WorldEntityTemplates::add(const char* name, const WorldEntity& entity) {
		SheetEntry entry;
		entry.hash = SID(name);
		entry.entity = entity;
		_entries.push_back(entry);
	}

	int WorldEntityTemplates::findIndex(const char* name) const {
		StaticHash hash = SID(name);
		for (uint32_t i = 0; i < _entries.size(); ++i) {
			if (_entries[i].hash == hash) {
				return i;
			}
		}
		XASSERT(1 == 2, "No matching spritesheet found for '%s'", name);
		return -1;
	}

	int WorldEntityTemplates::findIndex(const StaticHash& sid) const {
		for (uint32_t i = 0; i < _entries.size(); ++i) {
			if (_entries[i].hash == sid) {
				return i;
			}
		}
		XASSERT(1 == 2, "No matching spritesheet found for '%d'", sid);
		return -1;
	}

	bool WorldEntityTemplates::loadData(const JSONReader& loader) {
		int categories[256];
		int num = loader.get_categories(categories, 256);
		for (int i = 0; i < num; ++i) {
			const char* sn = loader.get_string(categories[i], "name");
			WorldEntity entity;
			loader.get(categories[i], "position", &entity.position);
			if (!loader.get(categories[i], "scale", &entity.scale)) {
				entity.scale = v3(1, 1, 1);
			}
			v3 rot;
			if (loader.get(categories[i], "rotation", &rot)) {
				for (int j = 0; j < 3; ++j) {
					entity.rotation.data[j] = DEGTORAD(rot.data[j]);
				}
			}
			Rect r;
			if (loader.get(categories[i], "texture", &r)) {
				entity.texture = math::buildTexture(r);
			}
			if (!loader.get(categories[i], "color", &entity.color)) {
				entity.color = Color::WHITE;
			}
			loader.get_int(categories[i], "type", &entity.type);
			add(sn, entity);

		}
		return true;
	}

	bool WorldEntityTemplates::reloadData(const JSONReader& loader) {
		int categories[256];
		int num = loader.get_categories(categories, 256);
		for (int i = 0; i < num; ++i) {
			const char* sn = loader.get_string(categories[i], "name");
			WorldEntity entity;
			loader.get(categories[i], "position", &entity.position);
			if (!loader.get(categories[i], "scale", &entity.scale)) {
				entity.scale = v3(1, 1, 1);
			}
			v3 rot;
			if (loader.get(categories[i], "rotation", &rot)) {
				for (int j = 0; j < 3; ++j) {
					entity.rotation.data[j] = DEGTORAD(rot.data[j]);
				}
			}
			Rect r;
			if (loader.get(categories[i], "texture", &r)) {
				entity.texture = math::buildTexture(r);
			}
			if (!loader.get(categories[i], "color", &entity.color)) {
				entity.color = Color::WHITE;
			}
			loader.get_int(categories[i], "type", &entity.type);
			int idx = findIndex(sn);
			if (idx == -1) {
				add(sn, entity);
			}
			else {
				SheetEntry& se = _entries[idx];
			}
		}
		return true;
	}
}