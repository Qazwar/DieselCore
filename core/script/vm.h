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

		enum ParserStatus {
			PS_OK,
			PS_ERROR,
			PS_NO_METHOD,
			PS_INVALID_FUNCTION,
			PS_NOT_ENOUGH_TOKENS,
			PS_MISSING_ASSIGNMENT,
			PS_UNKNOWN_REGISTER_TYPE,
			PS_WRONG_V4_DEFINITION,
			PS_NO_VAR_DEFINITION,
			PS_VAR_DEFINITION
		};

		struct Variable {
			VarType type;
			int index;
		};

		struct VMVariable {
			v4 value;
			StaticHash hash;
		};

		struct RegisterIndex {
			int index;
			int offset;

			RegisterIndex() : index(-1), offset(-1) {}
		};

		struct Line {
			RegisterIndex register_index;
			Operation operation;
			Variable var1;
			Variable var2;
		};

		enum FunctionType {
			FT_SIN,FT_COS,FT_LRP,FT_D2R,FT_SAT,FT_CLM,FT_TWN,FT_CLR
		};

		struct FunctionArgument {
			VarType type;
			int index;
		};

		struct Function {
			FunctionType type;
			int arguments;
			int function_index;
			FunctionArgument args[6];
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
			v4 execute();
			v4 execute(uint32_t index);
			v4 execute(StaticHash name);
			v4 execute(const Method& m);
			const v4& getRegister(int idx) const {
				return data[idx];
			}
			ParserStatus parse(const char* text);
			bool loadData(const char* text);
			bool reloadData(const char* text);
			void debugMethod(StaticHash hash);
			const ParserStatus& getStatus() const {
				return _status;
			}
		private:	
			int parseOperand(const char* data, const Tokenizer& t, int index, Variable* var);
			int parseNumber(const Tokenizer& t, int index, Variable* var);
			int parseNumber(const Tokenizer& t, int index, v4* var);
			int parseVariableDefinition(const char* data, Tokenizer& t, int index);
			int getMethod(const char* data, const Tokenizer& t, int index, Method* m);
			v4 get_data(const Variable& var);
			v4 executeFunction(const Function& f);
			int parseLine(const char* data, Tokenizer& t, int index, Line* line);
			int parseFunction(const char* data, const Tokenizer& t, int index, Variable* var);
			int parseOperand(Variable* var, const char* name, int index);
			int function_index(const char* name);
			int constant_index(const char* name);
			ParserStatus _status;
			RegisterIndex get_register(const char* t);
			v4 data[6];
			Array<Method> _methods;
			Array<VMVariable> variables;
			Array<VMVariable> constants;
			Array<v4> numbers;
			Array<Function> functions;

		};
	}

	

}