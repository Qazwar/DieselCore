#include "FileRepository.h"
#include "..\string\StringUtils.h"
#include "..\log\Log.h"
#include "TextCompressor.h"
#include "..\lib\collection_types.h"
#include "DataFile.h"
#include "..\io\FileUtils.h"
#include "..\string\StaticHash.h"
#include "BinaryFile.h"

namespace ds {

	namespace repository {

		// -----------------------------------------------------------
		// FileDescriptor
		// -----------------------------------------------------------
		struct FileDescriptor {

			StaticHash hash;
			int index;
			int size;
			bool binary;
			int name_index;
			DataFile* dataFile;
			FILETIME filetime;
			bool loaded;

		};

		// -----------------------------------------------------------
		// FileRepo
		// -----------------------------------------------------------
		struct FileRepo {

			RepositoryMode mode;
			CharBuffer name_buffer;
			Array<FileDescriptor> files;

		};

		static FileRepo* _repository = nullptr;

		// -----------------------------------------------------------
		// find files recursively in all directories
		// -----------------------------------------------------------
		void find_files(std::string& dir) {
			WIN32_FIND_DATAA ffd;
			HANDLE hFind = INVALID_HANDLE_VALUE;
			hFind = FindFirstFileA((dir + "\\*").c_str(), &ffd);
			if (INVALID_HANDLE_VALUE != hFind) {
				do {
					if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						std::string fn = dir + "\\" + std::string(ffd.cFileName);
						FileDescriptor fd;
						fd.hash = StaticHash(fn.c_str());
						fd.dataFile = nullptr;
						fd.index = -1;
						fd.size = -1;
						fd.loaded = false;
						fd.name_index = _repository->name_buffer.append(fn.c_str());
						file::getFileTime(fn.c_str(), fd.filetime);
						_repository->files.push_back(fd);
					}
					else {						
						if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0) {
							std::string nd = dir + "\\" + std::string(ffd.cFileName);
							find_files(nd);
						}						
					}
				} while (FindNextFileA(hFind, &ffd) != 0);
				FindClose(hFind);
			}
			else {
				LOGE << "cannot find directory " << dir;
			}
		}

		// -----------------------------------------------------------
		// intialize
		// -----------------------------------------------------------
		void initialize(RepositoryMode mode) {
			_repository = new FileRepo;
			_repository->mode = mode;
			if (mode == RM_RELEASE) {
				BinaryFile bf;
				if (bf.open("data\\e.pak", FileMode::READ)) {
					int sz = 0;
					bf.read(&sz);
					LOG << "repository entries: " << sz;
					for ( int i = 0; i < sz; ++i ) {
						FileDescriptor desc;
						uint32_t h = 0;
						bf.read(&h);
						desc.hash = StaticHash(h);
						bf.read(&desc.index);
						bf.read(&desc.size);
						_repository->files.push_back(desc);
						LOG << "hash: " << h << " index: " << desc.index << " size: " << desc.size;
					}
				}
			}
			else {
				find_files(std::string("content"));
				LOG << "total files: " << _repository->files.size();
				for (uint32_t i = 0; i < _repository->files.size(); ++i) {
					const FileDescriptor& fd = _repository->files[i];
					const char* name = _repository->name_buffer.data + fd.name_index;
					LOG << name << " = " << fd.hash.get();
				}
			}
		}

		// -----------------------------------------------------------
		// already loaded
		// -----------------------------------------------------------
		bool already_loaded(const char* fileName) {
			/*
			StaticHash hash = StaticHash(fileName);
			for (size_t i = 0; i < _repository->entries.size(); ++i) {
				const RepositoryEntry& entry = _repository->entries[i];
				if (entry.hash == hash) {
					return true;
				}
			}
			*/
			return false;
		}

		// -----------------------------------------------------------
		// find index of file
		// -----------------------------------------------------------
		int find_index(const StaticHash& hash) {
			int i = 0;
			for (const auto& entry : _repository->files) {
				if (entry.hash == hash) {
					return i;
				}
				++i;
			}
			return -1;
		}

		// -----------------------------------------------------------
		// load and store file info
		// -----------------------------------------------------------
		void load(DataFile* file, FileType type) {
			int size = 0;
			int idx = find_index(file->getFileName());
			if (idx != -1) {
				if (file->load()) {			
					_repository->files[idx].loaded = true;
					_repository->files[idx].dataFile = file;
				}
			}
		}

		// -----------------------------------------------------------
		// already in the watch list
		// -----------------------------------------------------------
		bool already_watching(const StaticHash& hash) {
			return find_index(hash) == -1;
		}

		// -----------------------------------------------------------
		// add DataFile to FileDescriptor
		// -----------------------------------------------------------
		void add(DataFile* file) {
			//if (!already_watching(file->getFileName())) {
				int idx = find_index(file->getFileName());
				if (idx != -1) {
					_repository->files[idx].dataFile = file;
				}
			//}
		}

		// -----------------------------------------------------------
		// reload (only in debug mode)
		// -----------------------------------------------------------
		void reload() {
			if (_repository->mode == RM_DEBUG) {
				TimeTracker tt("Reload");
				int reloaded = 0;
				for (auto& info : _repository->files) {
					const char* name = _repository->name_buffer.data + info.name_index;
					if (file::compareFileTime(name, info.filetime)) {
						HANDLE hData = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
						if (hData != INVALID_HANDLE_VALUE) {
							CloseHandle(hData);
							LOG << "Reloading file: " << name;
							info.dataFile->load();
							file::getFileTime(name, info.filetime);
							++reloaded;
						}
						else {
							LOGE << "Cannot read file: '" << name << "'";
						}
					}
				}
				if (reloaded > 0) {
					LOG << "Number of reloaded files: " << reloaded;
				}
			}
		}

		// -----------------------------------------------------------
		// load
		// -----------------------------------------------------------
		FileStatus load(File* file) {
			//XASSERT(_repository != 0,"No repository created yet");
			file->size = 0;
			int fidx = find_index(file->name);
			if (fidx == -1) {
				LOG << "No matching file found";
				return FS_NOT_FOUND;
			}
			FileDescriptor& fd = _repository->files[fidx];
			if (_repository->mode == RM_DEBUG) {
				const char* fileName = _repository->name_buffer.data + fd.name_index;
				FILE *fp = fopen(fileName, "rb");
				if (fp) {
					fd.loaded = true;
					LOG << "Loading '" << fileName << "'";
					fseek(fp, 0, SEEK_END);
					int sz = ftell(fp);
					fseek(fp, 0, SEEK_SET);
					LOG << "size: " << sz;
					file->data = new char[sz + 1];
					fread(file->data, 1, sz, fp);
					file->data[sz] = '\0';
					fclose(fp);
					file->size = sz;
					return FS_OK;
				}
				else {
					LOGE << "Cannot find file: '" << fileName << "'";
					return FS_NOT_FOUND;
				}
			}
			else {
				FILE* fp = fopen("data\\c.pak", "rb");
				if (fp) {
					LOG << "reading at " << fd.index << " size: " << fd.size;
					fseek(fp, fd.index, SEEK_SET);
					file->data = new char[fd.size + 1];
					fread(file->data, 1, fd.size, fp);
					if (!fd.binary) {
						LOG << "file is encoded";
						char* result = compression::decode(file->data);
						fclose(fp);
						delete[] file->data;
						file->data = result;
						file->size = fd.size;
						return FS_OK;
					}
					else {
						LOG << "raw data";
						file->data[fd.size] = '\0';
						file->size = fd.size;
						return FS_OK;
					}
				}
				else {
					LOGE << "Cannot find content PAK";
					return FS_NO_CPAK;
				}
			}
		}

		// -----------------------------------------------------------
		// load
		// -----------------------------------------------------------
		char* load(const StaticHash& fileName, int* size, FileType type) {
			//XASSERT(_repository != 0,"No repository created yet");
			*size = -1;
			int fidx = find_index(fileName);
			if (fidx == -1) {
				LOG << "No matching file found";
				return 0;
			}
			FileDescriptor& fd = _repository->files[fidx];
			if (_repository->mode == RM_DEBUG) {				
				const char* fileName = _repository->name_buffer.data + fd.name_index;
				FILE *fp = fopen(fileName, "rb");
				if (fp) {
					fd.loaded = true;
					LOG << "Loading '" << fileName << "'";
					fseek(fp, 0, SEEK_END);
					int sz = ftell(fp);
					fseek(fp, 0, SEEK_SET);
					LOG << "size: " << sz;
					char* buffer = new char[sz + 1];
					fread(buffer, 1, sz, fp);
					buffer[sz] = '\0';
					fclose(fp);
					*size = sz;
					return buffer;
				}
				else {
					LOGE << "Cannot find file: '" << fileName << "'";
					return 0;
				}
			}
			else {
				FILE* fp = fopen("data\\c.pak","rb");
				if (fp) {
					LOG << "reading at " << fd.index << " size: " << fd.size;
					fseek(fp, fd.index, SEEK_SET);
					char* buffer = new char[fd.size + 1];
					fread(buffer, 1, fd.size, fp);
					if (!fd.binary) {
						LOG << "file is encoded";
						char* result = compression::decode(buffer);
						fclose(fp);
						delete[] buffer;
						*size = fd.size;
						return result;
					}
					else {
						LOG << "raw data";
						buffer[fd.size] = '\0';
						*size = fd.size;
						return buffer;
					}
				}
				else {
					LOGE << "Cannot find content PAK";
					return 0;
				}
			}
			return 0;
		}

		// -----------------------------------------------------------
		// read file into char buffer
		// -----------------------------------------------------------
		char* read_file(const char* fileName,int* fileSize) {
			FILE *fp = fopen(fileName, "rb");
			if (fp) {
				fseek(fp, 0, SEEK_END);
				int sz = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				char* buffer = new char[sz + 1];
				fread(buffer, 1, sz, fp);
				//buffer[sz] = '\0';
				fclose(fp);
				*fileSize = sz;
				return buffer;
			}
			return 0;
		}

		// -----------------------------------------------------------
		// shutdown
		// -----------------------------------------------------------
		void shutdown() {
			if (_repository->mode == RM_DEBUG) {
				LOG << "total entries: " << _repository->files.size();
				BinaryFile cf;
				int index = 0;
				if (cf.open("data\\c.pak", FileMode::WRITE)) {
					for (auto& entry : _repository->files) {
						if (entry.loaded) {
							entry.index = index;
							// read file
							int fileSize = -1;
							const char* name = _repository->name_buffer.data + entry.name_index;
							char* content = read_file(name, &fileSize);
							LOG << "adding file: " << name << " index: " << index << " file size: " << fileSize << " hash: " << entry.hash.get();
							if (content != 0) {
								int counter = 0;
								// write buffer
								for (int j = 0; j < fileSize; ++j) {
									cf.write(content[j]);
									++counter;
								}
								entry.size = counter;
								index += counter;
								delete[] content;
							}
						}
					}
				}
				// save directory
				BinaryFile bf;
				int total = 0;
				for (uint32_t i = 0; i < _repository->files.size(); ++i) {
					if (_repository->files[i].loaded) {
						++total;
					}
				}
				LOG << "number of added files: " << total;
				LOG << "saving e.pak";
				if (bf.open("data\\e.pak", FileMode::WRITE)) {
					bf.write(total);
					for (int i = 0; i < _repository->files.size(); ++i) {
						const FileDescriptor& entry = _repository->files[i];
						if (entry.loaded) {
							bf.write(entry.hash.get());
							bf.write(entry.index);
							bf.write(entry.size);
						}
					}
				}
			}
			delete _repository;
		}

		// -----------------------------------------------------------
		// list entries of e.pak
		// -----------------------------------------------------------
		void list() {
			/*
			Array<RepositoryEntry> entries;
			FILE* f = fopen("data\\e.pak", "rb");
			if (f) {
				int sz = 0;
				fread(&sz, sizeof(int), 1, f);
				for (int i = 0; i < sz; ++i) {
					RepositoryEntry entry;
					fread(&entry.hash, sizeof(StaticHash), 1, f);
					fread(&entry.size, sizeof(int), 1, f);
					fread(&entry.index, sizeof(int), 1, f);
					entries.push_back(entry);
				}
				fclose(f);
			}
			for (size_t i = 0; i < entries.size(); ++i) {
				const RepositoryEntry& entry = entries[i];
				const char* name = _repository->name_buffer.data + entry.name_index;
				LOG << name << " size: " << entry.size << " index: " << entry.index;
				FILE* fp = fopen("data\\c.pak", "rb");
				if (fp) {
					fseek(fp, entry.index, SEEK_SET);
					char* buffer = new char[entry.size + 1];
					fread(buffer, 1, entry.size, fp);
					char* result = compression::decode(buffer);
					int len = strlen(result);
					fclose(fp);
					delete[] buffer;
					LOG << "##### len: " << len;
					LOG << "RESULT: '" << result << "'";
					delete[] result;
				}
			}
			*/
		}
	}

	

}
