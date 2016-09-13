#pragma once
#include "DataFile.h"
#include "..\profiler\Profiler.h"
#include "..\log\Log.h"
#include "json.h"
#include "FileRepository.h"

namespace ds {

	
	// -----------------------------------------------
	// load JSON
	// -----------------------------------------------
	bool JSONAssetFile::load() {
		bool ret = false;
		// check length
		JSONReader reader;
		if (reader.parse(_hash)) {
			if (_loaded) {
				ret = reloadData(reader);
			}
			else {
				ret = loadData(reader);
				if (ret) {
					_loaded = true;
				}
			}
			repository::add(this);
		}
		else {
			LOGE << "Error: Cannot parse file";
			ret = false;
		}
		return ret;
	}

	// -----------------------------------------------
	// load JSON
	// -----------------------------------------------
	bool FlatJSONAssetFile::load() {
		bool ret = false;
		// check length
		StopWatch s;
		s.start();
		LOG << "Reading simplified json file: " << _hash.get();
		FlatJSONReader reader;
		if (reader.parse(_hash)) {
			if (_loaded) {
				LOG << "-> Reloading";
				ret = reloadData(reader);
			}
			else {
				LOG << "-> Loading";
				ret = loadData(reader);
				if (ret) {
					_loaded = true;
				}
			}
			repository::add(this);
		}
		else {
			LOG << "Error: Cannot parse file: " << _hash.get();
			ret = false;
		}
		s.end();
		LOG << "----> elapsed: " << s.elapsed();
		return ret;
	}


}