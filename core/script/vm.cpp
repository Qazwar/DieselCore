#include "vm.h"
#include <stdio.h>
#include "..\io\json.h"
#include <stdint.h>

namespace ds {

	namespace vm {

		struct FunctionDefinition {

			FunctionType type;
			const char* name;
			int arguments;

			FunctionDefinition(FunctionType t, const char* n, int a) : type(t) , name(n), arguments(a) {}

		};

		FunctionDefinition FUNCTIONS[] = {
			{FT_SIN, "SIN", 1 },
			{FT_COS, "COS", 1 }
		};

		int get_register(const char* t) {
			if (t[0] == 'R') {
				return t[1] - '0';
			}
			return -1;
		}

		int function_index(const char* name) {
			char t[4];
			for (int i = 0; i < 3; ++i) {
				t[i] = name[i];
			}
			t[3] = '\0';
			for (int i = 0; i < 2; ++i) {
				if (strcmp(FUNCTIONS[i].name, t) == 0) {
					return i;
				}
			}
			return -1;
		}

		int parseOperand(VMContext* context, Variable* var, const char* name, int index) {
			int idx = index;
			int fi = function_index(name);
			if (fi != -1) {
				printf("FOUND FUNCTION!!!\n");
				var->type = VT_FUNCTION;
				var->index = fi;
			}
			else {
				int ri = get_register(name);
				if (ri > 0 && ri < 5) {
					var->type = VT_REG;
					var->index = ri;
				}
				else {
					var->type = VT_VARIABLE;
					int idx = -1;
					StaticHash hash(name);
					for (uint32_t i = 0; i < context->variables.size(); ++i) {
						if (context->variables[i].hash == hash) {
							idx = i;
						}
					}
					if (idx != -1) {
						var->index = idx;
					}
				}
			}
			// can be variable or function
			++idx;
			return idx;
		}

		int parseFunction(VMContext* context, const char* data, const Tokenizer& t, int index, Variable* var) {
			int idx = index + 1;
			char name[128] = { '\0' };
			const FunctionDefinition& def = FUNCTIONS[var->index];
			for (int i = 0; i < def.arguments; ++i) {				
				const Token& tok = t.get(idx);
				Function f;
				f.type = def.type;
				f.arguments = def.arguments;
				f.function_index = var->index;
				FunctionArgument& arg = f.args[i];
				if (tok.type == Token::NAME) {
					strncpy(name, data + tok.index, tok.size);
					int ri = get_register(name);
					if (ri > 0 && ri < 5) {
						arg.type = VT_REG;
						arg.index = ri;
					}
					else {
						arg.type = VT_VARIABLE;
						int idx = -1;
						StaticHash hash(name);
						for (uint32_t i = 0; i < context->variables.size(); ++i) {
							if (context->variables[i].hash == hash) {
								idx = i;
							}
						}
						if (idx != -1) {
							arg.index = idx;
						}
					}
				}
				else {
					arg.type = VT_CONSTANT;
					arg.index = context->add(v4(tok.value));
				}
				idx += 2;
				context->functions.push_back(f);
				var->index = context->functions.size() - 1;
			}
			++idx;
			return idx;
		}

		int parseLine(const char* data, VMContext* context, Tokenizer& t, int index, Line* line) {
			char name[128] = { '\0' };
			Token& tok = t.get(index);
			strncpy(name, data + tok.index, tok.size);
			line->register_index = get_register(name);
			++index;
			++index; // ASSIGN
			tok = t.get(index);
			if (tok.type == Token::NAME) {
				strncpy(name, data + tok.index, tok.size);
				index = parseOperand(context, &line->var1, name, index);
				if (line->var1.type == VT_FUNCTION) {
					index = parseFunction(context, data, t, index, &line->var1);
				}
			}
			else if (tok.type == Token::NUMBER) {
				Variable v1;
				v1.type = VT_CONSTANT;
				v1.index = context->add(v4(tok.value));
				line->var1 = v1;
				++index;
			}
			else {
				++index;
			}
			tok = t.get(index);
			switch (tok.type) {
				case Token::PLUS: line->operation = OP_PLUS; break;
				case Token::MINUS: line->operation = OP_MINUS; break;
				case Token::MULTIPLY: line->operation = OP_MUL; break;
				case Token::SLASH: line->operation = OP_DIV; break;
				default: line->operation = OP_NONE;
			}
			++index;
			tok = t.get(index);
			if (tok.type == Token::NAME) {
				strncpy(name, data + tok.index, tok.size);
				index = parseOperand(context, &line->var2, name, index);
				if (line->var2.type == VT_FUNCTION) {
					index = parseFunction(context, data, t, index, &line->var2);
				}
			}
			else if (tok.type == Token::NUMBER) {
				Variable v;
				v.type = VT_CONSTANT;
				v.index = context->add(v4(tok.value));
				line->var2 = v;
				++index;
			}
			else {
				++index;
			}
			tok = t.get(index);
			if (tok.type == Token::SEMICOLON) {
				++index;
				++index;
			}
			else {
				++index;
			}
			return index;
		}

		// -------------------------------------------------------
		// parse
		// -------------------------------------------------------
		void parse(const char* text, VMContext* context) {
			Tokenizer t;
			t.parse(text);
			int n = 0;
			while (n < t.size()) {
				Line line;
				n = parseLine(text, context, t, n, &line);
				context->lines.push_back(line);
			}
		}

		v4 executeFunction(VMContext* context,const Function& f) {
			v4 ret(0, 0, 0, 0);
			if (f.type == FT_SIN) {
				const FunctionArgument& arg = f.args[0];
				v4 s(0, 0, 0, 0);
				if (arg.type == VT_CONSTANT) {
					s = context->constants[arg.index];
				}
				else if (arg.type == VT_REG) {
					s = context->data[arg.index];
				}
				else if (arg.type == VT_VARIABLE) {
					s =  context->variables[arg.index].value;
				}
				for (int i = 0; i < 4; ++i) {
					ret.data[i] = sin(s.data[i]);
				}
			}
			return ret;
		}
		// -------------------------------------------------------
		// get data from context by variable
		// -------------------------------------------------------
		v4 get_data(VMContext* context, const Variable& var) {
			if (var.type == VT_CONSTANT) {
				return context->constants[var.index];
			}
			else if (var.type == VT_REG) {
				return context->data[var.index];
			}
			else if (var.type == VT_VARIABLE) {
				return context->variables[var.index].value;
			}
			else if (var.type == VT_FUNCTION) {
				return executeFunction(context, context->functions[var.index]);
			}
			return v4(0, 0, 0, 0);
		}

		// -------------------------------------------------------
		// execute
		// -------------------------------------------------------
		void execute(VMContext* context) {
			for (int i = 0; i < context->lines.size(); ++i) {
				const Line& l = context->lines[i];
				int oi = l.register_index;
				// simple assignment
				if (l.operation == Operation::OP_NONE) {
					Variable v = l.var1;
					if (v.type == VT_CONSTANT) {
						context->data[oi] = context->constants[v.index];
					}
				}
				else {
					v4 t1 = get_data(context, l.var1);
					v4 t2 = get_data(context, l.var2);					
					switch (l.operation) {
						case OP_PLUS: context->data[oi] = t1 + t2; break;
						case OP_MINUS: context->data[oi] = t1 - t2; break;
						case OP_DIV: 
							for (int i = 0; i < 4; ++i) {
								context->data[oi].data[i] = t1.data[i] / t2.data[i];
							}
							break;
						case OP_MUL: 
							for (int i = 0; i < 4; ++i) {
								context->data[oi].data[i] = t1.data[i] * t2.data[i];
							}
							break;
					}
				}
				
			}
		}

	}

}