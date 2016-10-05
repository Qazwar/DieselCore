#pragma once
#include "..\Common.h"
#include "..\lib\collection_types.h"
#include "..\io\ReportWriter.h"

namespace ds {

	struct AdditionalDataHeader {
		int index;
		int size;
		bool used;
		ID sid;
		int identifier;
	};

	class AdditionalData {

	public:
		AdditionalData() {}
		~AdditionalData() {}
		void* attach(ID sid, int size,int identifier);
		void remove(ID sid);
		void* get(ID sid);
		bool contains(ID sid);
		void debug();
		void save(const ReportWriter& writer);
	private:
		int find_free_header(int size);
		int find_header(ID sid);
		CharBuffer data;
		Array<AdditionalDataHeader> headers;
	};

}