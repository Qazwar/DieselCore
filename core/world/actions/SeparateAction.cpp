#include "SeparateAction.h"
#include "..\..\log\Log.h"
#include "..\..\math\math.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	SeparateAction::SeparateAction(ChannelArray* array, const Rect& boundingRect) : AbstractAction(array, boundingRect, "separate") {
		int sizes[] = { sizeof(ID), sizeof(int), sizeof(float), sizeof(float) };
		_buffer.init(sizes, 4);
	}

	void SeparateAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (ID*)_buffer.get_ptr(0);
			_types = (int*)_buffer.get_ptr(1);
			_minDistances = (float*)_buffer.get_ptr(2);
			_relaxations = (float*)_buffer.get_ptr(3);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void SeparateAction::attach(ID id, int type, float minDistance, float relaxation) {
		int idx = create(id);
		_ids[idx] = id;
		_types[idx] = type;
		_minDistances[idx] = minDistance;
		_relaxations[idx] = relaxation;
	}

	int SeparateAction::find_by_type(int type, ID* ids, int max) const {
		int* indices = _array->_sparse;
		int cnt = 0;
		for (int i = 0; i < _array->capacity; ++i) {
			if (indices[i] != -1 && cnt < max) {
				int t = _array->get<int>(i, WEC_TYPE);
				if (t == type) {
					ids[cnt++] = i;
				}
			}
		}
		return cnt;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void SeparateAction::update(float dt,ActionEventBuffer& buffer) {
		ID ids[256];
		if (_buffer.size > 0) {
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				float sqrDist = _minDistances[i] * _minDistances[i];
				v3 f = _array->get<v3>(_ids[i], WEC_FORCE);
				v3 currentPos = _array->get<v3>(_ids[i], WEC_POSITION);
				int num = find_by_type(_types[i], ids, 256);
				for (int k = 0; k < num; ++k) {
					if (_ids[i] != ids[k]) {
						v3 dist = _array->get<v3>(ids[k], WEC_POSITION) - currentPos;
						if (sqr_length(dist) < sqrDist) {
							v3 separationForce = dist;
							separationForce = normalize(separationForce);
							separationForce = separationForce * _relaxations[i];
							f -= separationForce;
						}
					}
				}
				_array->set<v3>(_ids[i], WEC_FORCE, f);
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void SeparateAction::saveReport(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.addSubHeader("SeparateAction");
			const char* HEADERS[] = { "Index", "ID", "Type", "Min distance", "Relaxation" };
			writer.startTable(HEADERS, 5);
			for (uint32_t i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(i);
				writer.addCell(_ids[i]);
				writer.addCell(_types[i]);
				writer.addCell(_minDistances[i]);
				writer.addCell(_relaxations[i]);
				writer.endRow();
			}
			writer.endTable();
		}
	}

}