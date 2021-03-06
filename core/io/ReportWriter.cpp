#include "ReportWriter.h"
#include "..\profiler\Profiler.h"
#include "..\log\Log.h"

namespace ds {

	ReportWriter::ReportWriter(const char* fileName) {
		_watch = new StopWatch;
		_watch->start();
		_file = fopen(fileName, "w");
		if (_file) {
			fprintf(_file, "<!doctype html>\n<html>\n<head>\n");
			fprintf(_file, "<style>\n");
			fprintf(_file, "body{ padding-top:20px; padding-bottom:20px; font:Verdana 10px }h1, h2{ font-family:Verdana }#box-table-a th, .panel.panel-heading h1, h1, h2{ font-weight:700 }h1{ font-size:18px }\n");
			fprintf(_file, "h2{ font-size:15px }#box-table-a{ font-family:Verdana,\"Lucida Sans Unicode\",\"Lucida Grande\",Sans-Serif; font-size:12px; margin:5px; min-width:300px; text-align:left;\n");
			fprintf(_file, "border-collapse:collapse }#box-table-a th{ font-size:13px; padding:4px; background:#b9c9fe; border-bottom:1px solid #fff; color:#039; border-left:2px solid #fff }\n");
			fprintf(_file, "#box-table-a td{ padding:8px; background:#e8edff; border-bottom:2px solid #fff; border-left:2px solid #fff; color:#669; border-top:1px solid transparent }\n");
			fprintf(_file, "#box-table-a tr : hover td{ background:#d0dafd; color:#339 }.panel{ margin-bottom:20px; background-color:#fff; border:1px solid #ddd; border-radius:4px; -webkit-box-shadow:0 1px 1px rgba(0,0,0,.05); \n");
			fprintf(_file, "box-shadow:0 1px 1px rgba(0,0,0,.05) }body.panel.panel-heading{ color:grey; font-weight:700 }.panel.panel-heading{ color:#333; background-color:#f5f5f5; \n");
			fprintf(_file, "border-color:#ddd }.panel-heading{ padding:10px 15px; border-bottom:1px solid transparent; border-top-right-radius:3px; border-top-left-radius:3px }\n");
			fprintf(_file, ".panel.panel-heading h1{ font-family:Verdana; font-size:18px; color:#333; padding:2px; margin:0 }\n");
			fprintf(_file, "</style>\n");
			fprintf(_file, "</head><body>\n");
		}
		_open = true;
	}

	ReportWriter::~ReportWriter() {
		if (_file) {
			_watch->end();
			startBox("Report");
			const char* headers[] = { "Type","Time" };
			startTable(headers,2);
			startRow();
			addCell("Report");
			addCell((float)_watch->elapsedMS());
			endRow();
			startRow();
			addCell("Perf");
			addCell(perf::get_current_total_time());
			endRow();
			endTable();
			endBox();
			fprintf(_file, "</body></html>\n");
			fclose(_file);
		}
		delete _watch;
	}
	void ReportWriter::addHeader(const char* header) const {
		fprintf(_file, "<h1>%s</h1>\n", header);
	}

	void ReportWriter::startBox(const char* header) const {
		fprintf(_file, "<div class = 'panel'>\n<div class = 'panel-heading'><h1>%s</h1></div>\n", header);
	}

	void ReportWriter::endBox() const {
		fprintf(_file, "</div>\n");
	}

	void ReportWriter::addSubHeader(const char* header) const {
		fprintf(_file, "<h2>%s</h2>\n", header);
	}

	void ReportWriter::startTable(const char** columnNames, int num) const {
		fprintf(_file, "<table id = 'box-table-a'><thead>  <tr>");
		for (int i = 0; i < num; ++i) {
			fprintf(_file, "    <th scope='col'>%s</th>\n", columnNames[i]);
		}
		fprintf(_file, "  </tr></thead><tbody>\n");
	}

	void ReportWriter::startRow() const {
		fprintf(_file, "  <tr>\n");
	}

	void ReportWriter::addCell(int v) const {
		fprintf(_file, "    <td>%d</td>\n", v);
	}

	void ReportWriter::addCell(uint32_t v) const {
		fprintf(_file, "    <td>%d</td>\n", v);
	}

	void ReportWriter::addCell(const v2& v) const {
		fprintf(_file, "    <td>x: %g  y: %g</td>\n", v.x, v.y);
	}

	void ReportWriter::addCell(const v3& v) const {
		fprintf(_file, "    <td>x: %g  y: %g z: %g</td>\n", v.x, v.y, v.z);
	}

	void ReportWriter::addCell(const Rect& r) const {
		fprintf(_file, "    <td>t: %g  l: %g w: %g h: %g</td>\n", r.top, r.left, r.width(), r.height());
	}

	void ReportWriter::addCell(const Texture& r) const {
		addCell(r.rect);
	}

	void ReportWriter::addCell(const Color& c) const {
		int r = c.r * 255.0f;
		int g = c.g * 255.0f;
		int b = c.b * 255.0f;
		int a = c.a * 255.0f;
		fprintf(_file, "    <td>r: %d  g: %d b: %d a: %d</td>\n", r, g , b ,a);
	}

	void ReportWriter::addCell(float v) const {
		fprintf(_file, "    <td>%g</td>\n", v);
	}

	void ReportWriter::addCell(const char* v) const {
		fprintf(_file, "    <td>%s</td>\n", v);
	}

	void ReportWriter::addCell(int ident, const char* v) const {
		fprintf(_file, "    <td>");
		for (int i = 0; i < ident; ++i) {
			fprintf(_file, "-&nbsp;");
		}
		fprintf(_file, "%s</td>\n", v);
	}

	void ReportWriter::addCell(bool v) const {
		if (v) {
			fprintf(_file, "    <td>true</td>\n");
		}
		else {
			fprintf(_file, "    <td>false</td>\n");
		}
	}

	void ReportWriter::endRow() const {
		fprintf(_file, "  </tr>\n");
	}

	void ReportWriter::endTable() const {
		fprintf(_file, "</tbody>\n</table>\n");
	}


}