#pragma once
#include <Vector.h>
#include "..\string\StaticHash.h"
#include "..\lib\collection_types.h"

namespace ds {

	namespace vm {

		enum VarType {
			VT_REG,
			VT_CONSTANT,
			VT_FUNCTION,
			VT_VARIABLE
		};

		enum Operation {
			OP_PLUS,
			OP_MINUS,
			OP_MUL,
			OP_DIV,
			OP_NONE
		};

		struct Variable {
			VarType type;
			int index;
		};

		struct VMVariable {
			v4 value;
			StaticHash hash;
		};

		struct Line {
			int register_index;
			Operation operation;
			Variable var1;
			Variable var2;
		};

		enum FunctionType {
			FT_SIN,FT_COS
		};

		struct FunctionArgument {
			VarType type;
			int index;
		};

		struct Function {
			FunctionType type;
			int arguments;
			int function_index;
			FunctionArgument args[4];
		};

		struct VMContext {
			v4 data[6];
			Array<Line> lines;
			Array<VMVariable> variables;
			Array<v4> constants;
			Array<Function> functions;

			void set(StaticHash hash, const v4& v) {
				int idx = -1;
				for (uint32_t i = 0; i < variables.size(); ++i) {
					if (variables[i].hash == hash) {
						idx = i;
					}
				}
				if (idx == -1) {
					VMVariable c;
					c.hash = hash;
					c.value = v;
					variables.push_back(c);
				}
				else {
					VMVariable& c = variables[idx];
					c.value = v;
				}
			}

			int add(const v4& v) {
				constants.push_back(v);
				return constants.size() - 1;
			}
		};

		void parse(const char* text,VMContext* context);

		void execute(VMContext* context);

	}

}