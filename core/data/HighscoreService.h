#pragma once
#include "..\lib\collection_types.h"
#include "..\io\BinaryFile.h"

namespace ds {

	template<class H, int SIZE = 10>
	struct Highscores {

		Highscores() : _count(0) {
			_scores = new H[SIZE];
		}

		~Highscores() {
			delete[] _scores;
		}

		int add(const H& score) {
			int idx = -1;
			if (_count == 0) {
				_scores[_count++] = score;
				return 0;
			}
			for (int i = 0; i < _count; ++i) {
				if (idx == -1) {
					const H& other = _scores[i];
					int state = score.compare(other);
					if (state == -1) {
						idx = i;
					}
				}
			}
			if (idx != -1) {
				for (int j = _count - 1; j >= idx; --j) {
					if (j < SIZE - 1) {
						_scores[j + 1] = _scores[j];
					}
				}
				_scores[idx] = score;
				if (_count < SIZE) {
					++_count;
				}
			}
			else if (_count < SIZE) {
				_scores[_count++] = score;
				idx = _count - 1;
			}
			return idx;
		}

		void set(int index, const H& h) {
			_scores[index] = h;
		}

		const H& get(int index) const {
			return _scores[index];
		}

		int get(H* list, int max) {
			int cnt = max;
			if (cnt > _count) {
				cnt = _count;
			}
			for (int i = 0; i < cnt; ++i) {
				list[i] = _scores[i];
			}
			return cnt;
		}

		int size() const {
			return _count;
		}
		int _count;
		H* _scores;
	};
	/*
	template<class H,int SIZE = 10>
	class HighscoreService {

	public:
		HighscoreService() : _count(0) {
			_scores = new H;
		}

		~HighscoreService() {
			delete _scores;
		}

		int add(const H& score) {
			return _scores->add(score);
		}

		bool load(const char* name) {
			BinaryFile f;
			if (f.open(name, FileMode::READ)) {
				f.read(&_count);
				if (_count > SIZE) {
					_count = SIZE;
				}
				for (int i = 0; i < _count; ++i) {
					f.read(&_scores[i], sizeof(H));
				}			
				return true;
			}
			return false;
		}
		
		bool save(const char* name) {
			BinaryFile f;
			if (f.open(name, FileMode::WRITE)) {
				f.write(_count);
				for (int i = 0; i < _count; ++i) {
					f.write(&_scores[i], sizeof(H));
				}			
				return true;
			}
			return false;
		}

		const H& get(int index) const {
			return _scores[index];
		}

		int get(H* list, int max) {
			return _scores->get(list, max);
		}

		int size() const {
			return _count;
		}
	private:
		int _count;
		H* _scores;
	};

	template<class H, int SIZE = 10, int BUCKETS = 5>
	class BucketHighscoreService {

	public:
		HighscoreService() : _count(0) {
			_scores = new H[SIZE];
		}

		~HighscoreService() {
			delete[] _scores;
		}

		int add(const H& score) {
			int idx = -1;
			if (_count == 0) {
				_scores[_count++] = score;
				return 0;
			}
			for (int i = 0; i < _count; ++i) {
				if (idx == -1) {
					int state = score.compare(_scores[i]);
					if (state == -1) {
						idx = i;
					}
				}
			}
			if (idx != -1) {
				for (int j = _count - 1; j >= idx; --j) {
					if (j < SIZE - 1) {
						_scores[j + 1] = _scores[j];
					}
				}
				_scores[idx] = score;
				if (_count < SIZE) {
					++_count;
				}
			}
			else if (_count < SIZE) {
				_scores[_count++] = score;
				idx = _count - 1;
			}
			return idx;
		}

		bool load(const char* name) {
			BinaryFile f;
			if (f.open(name, FileMode::READ)) {
				f.read(&_count);
				if (_count > SIZE) {
					_count = SIZE;
				}
				for (int i = 0; i < _count; ++i) {
					f.read(&_scores[i], sizeof(H));
				}
				return true;
			}
			return false;
		}

		bool save(const char* name) {
			BinaryFile f;
			if (f.open(name, FileMode::WRITE)) {
				f.write(_count);
				for (int i = 0; i < _count; ++i) {
					f.write(&_scores[i], sizeof(H));
				}
				return true;
			}
			return false;
		}

		const H& get(int index) const {
			return _scores[index];
		}

		int get(H* list, int max) {
			int cnt = max;
			if (cnt > _count) {
				cnt = _count;
			}
			for (int i = 0; i < cnt; ++i) {
				list[i] = _scores[i];
			}
			return cnt;
		}

		int size() const {
			return _count;
		}
	private:
		int _count;
		H* _scores;
	};
	*/
}