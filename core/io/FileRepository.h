#pragma once
#include <stdint.h>
#include "..\string\StaticHash.h"

namespace ds {

	class DataFile;

	enum FileType {
		FT_TEXT,
		FT_BINARY,
		FT_EOL
	};

	enum FileStatus {
		FS_OK,
		FS_ERROR,
		FS_NOT_FOUND,
		FS_NO_CPAK
	};

	struct File {

		StaticHash name;
		char* data;
		uint32_t size;		
		FileType type;

		File() : name((uint32_t)0), data(0), size(0), type(FT_TEXT) {}

		explicit File(const StaticHash& n) : name(n), data(0), size(0), type(FT_TEXT) {}

		File(const StaticHash& n, FileType t) : name(n), data(0), size(0), type(t) {}

		~File() {
			if (data != 0) {
				delete[] data;
			}
		}
	};

	namespace repository {

		enum RepositoryMode {
			RM_DEBUG,
			RM_RELEASE
		};

		void initialize(RepositoryMode mode);

		void shutdown();

		FileStatus load(File* file);

		void add(DataFile* file);

		void reload();

		void list();
	}

}

