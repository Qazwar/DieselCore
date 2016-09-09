#include "TileMapReader.h"
#include<iostream>
#include<fstream>
#include <string>
#include "..\log\Log.h"
#include "..\io\json.h"
#include "..\io\FileRepository.h"

namespace ds {

	TileMapReader::TileMapReader() {
	}

	TileMapReader::~TileMapReader() {
	}

	bool TileMapReader::parse(const char* fileName) {
		ds::File f(SID(fileName));
		if (ds::repository::load(&f) == ds::FileStatus::FS_OK) {
			ds::Tokenizer t;
			t.parse(f.data);
			LOG << "file: " << fileName;
			LOG << "Tokens: " << t.size();
			_sizeY = 0;
			_sizeX = 0;
			for (int i = 0; i < t.size(); ++i) {
				ds::Token& tok = t.get(i);
				if (tok.type == ds::Token::NUMBER) {
					_data.push_back(static_cast<uint16_t>(tok.value));
				}
				else if (tok.type == ds::Token::NEWLINE) {
					if (_sizeX == 0) {
						_sizeX = i;
					}
					++_sizeY;
				}
			}
			++_sizeY;
			LOG << "SX: " << _sizeX << " SY:" << _sizeY;
			return true;
		}
		return false;
	}

	uint16_t TileMapReader::width() const {
		return _sizeX;
	}

	uint16_t TileMapReader::height() const {
		return _sizeY;
	}

	uint16_t TileMapReader::get(uint16_t x, uint16_t y) const {
		return _data[x + y * _sizeX];
	}

	uint16_t TileMapReader::get(uint16_t index) const {
		return _data[index];
	}

}