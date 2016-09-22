#pragma once
#include <Vector.h>
#include "..\string\StaticHash.h"
#include "..\lib\collection_types.h"
#include "..\io\DataFile.h"

namespace ds {

	namespace vm {

		enum VarType {
			VT_REG,
			VT_CONSTANT,
			VT_FUNCTION,
			VT_VARIABLE,
			VT_NUMBER,
			VT_NONE
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
			FT_SIN,FT_COS,FT_LRP,FT_D2R,FT_SAT,FT_CLM
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
	
		struct Method {
			StaticHash hash;
			Array<Line> lines;
		};
		// ------------------------------------------------------
		// Script
		// ------------------------------------------------------
		class Script : public TextAssetFile {

		public:
			Script(const char* name) : TextAssetFile(name) {
				addConstant(SID("TWO_PI"), v4(TWO_PI));
				addConstant(SID("PI"), v4(PI));
				addConstant(SID("HALF_PI"), v4(HALF_PI));
			}
			~Script() {}
			int registerVar(StaticHash hash);
			void set(int idx, const v4& v);
			void set(StaticHash hash, const v4& v);
			int addConstant(StaticHash hash, const v4& v);
			int add(const v4& v) {
				numbers.push_back(v);
				return numbers.size() - 1;
			}
			const Method& getMethod(StaticHash hash) const;
			void execute();
			void execute(uint32_t index);
			void execute(StaticHash name);
			void execute(const Method& m);
			const v4& getRegister(int idx) const {
				return data[idx];
			}
			void parse(const char* text);
			bool loadData(const char* text);
			bool reloadData(const char* text);
			void debugMethod(StaticHash hash);
		private:					
			int getMethod(const char* data, const Tokenizer& t, int index, Method* m);
			v4 get_data(const Variable& var);
			v4 executeFunction(const Function& f);
			int parseLine(const char* data, Tokenizer& t, int index, Line* line);
			int parseFunction(const char* data, const Tokenizer& t, int index, Variable* var);
			int parseOperand(Variable* var, const char* name, int index);
			int function_index(const char* name);
			int constant_index(const char* name);
			int get_register(const char* t);
			v4 data[6];
			Array<Method> _methods;
			Array<VMVariable> variables;
			Array<VMVariable> constants;
			Array<v4> numbers;
			Array<Function> functions;

		};
	}

	

}