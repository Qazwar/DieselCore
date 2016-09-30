#pragma once
#include <stdint.h>
#include <stdio.h>
#include <Vector.h>
#include "..\math\math_types.h"
#include "..\graphics\Texture.h"
#include "..\graphics\Color.h"

namespace ds {

	class ReportWriter {

	public:
		ReportWriter(const char* fileName);
		~ReportWriter();
		void addHeader(const char* header) const;
		void startBox(const char* header) const;
		void endBox() const;
		void addSubHeader(const char* header) const;
		void startTable(const char** columnNames, int num) const;
		void startRow() const;
		void addCell(int v) const;
		void addCell(uint32_t v) const;
		void addCell(const v2& v) const;
		void addCell(const v3& v) const;
		void addCell(const Rect& v) const;
		void addCell(const Texture& v) const;
		void addCell(const Color& v) const;
		void addCell(bool v) const;
		void addCell(float v) const;
		void addCell(const char* v) const;
		void addCell(int ident, const char* v) const;
		void endRow() const;
		void endTable() const;
		bool isOpen() const {
			return _open;
		}
	private:
		FILE* _file;
		bool _open;
	};

}