#include "vm.h"
#include <stdio.h>
#include "..\io\json.h"
#include <stdint.h>
#include <stack>

namespace ds {

	namespace vm {

		// -------------------------------------------------------
		// function pointer
		// -------------------------------------------------------
		typedef v4(*VMFunc)(v4*, int);

		// -------------------------------------------------------
		// vm SIN
		// -------------------------------------------------------
		v4 vm_sin(v4* args, int num) {
			v4 ret(0.0f);
			for (int i = 0; i < 4; ++i) {
				ret.data[i] = sin(args[0].data[i]);
			}
			return ret;
		}

		// -------------------------------------------------------
		// vm COS
		// -------------------------------------------------------
		v4 vm_cos(v4* args, int num) {
			v4 ret(0.0f);
			for (int i = 0; i < 4; ++i) {
				ret.data[i] = cos(args[0].data[i]);
			}
			return ret;
		}

		// -------------------------------------------------------
		// vm LERP
		// -------------------------------------------------------
		v4 vm_lerp(v4* args, int num) {
			v4 ret(0.0f);
			for (int i = 0; i < 4; ++i) {
				ret.data[i] = (1.0f - args[2].data[i]) * args[0].data[i] + args[2].data[i] * args[1].data[i];
			}
			return ret;
		}

		// -------------------------------------------------------
		// vm DEG2RAD
		// -------------------------------------------------------
		v4 vm_degtorad(v4* args, int num) {
			v4 ret(0.0f);
			for (int i = 0; i < 4; ++i) {
				ret.data[i] = DEGTORAD(args[0].data[i]);
			}
			return ret;
		}

		// -------------------------------------------------------
		// function definition
		// -------------------------------------------------------
		struct FunctionDefinition {

			FunctionType type;
			const char* name;
			int arguments;
			VMFunc function;

			FunctionDefinition(FunctionType t, const char* n, int a,VMFunc f) : type(t) , name(n), arguments(a) , function(f) {}

		};

		// -------------------------------------------------------
		// all functions
		// -------------------------------------------------------
		FunctionDefinition FUNCTIONS[] = {
			{ FT_SIN, "SIN", 1, &vm_sin},
			{ FT_COS, "COS", 1, &vm_cos},
			{ FT_LRP, "LRP", 3, &vm_lerp},
			{ FT_D2R, "D2R", 3, &vm_degtorad }
		};

		const int NUM_FUNCTIONS = 4;

		// -------------------------------------------------------
		// Script
		// -------------------------------------------------------

		// -------------------------------------------------------
		// extract register
		// -------------------------------------------------------
		int Script::get_register(const char* t) {
			if (t[0] == 'R') {
				return t[1] - '0';
			}
			return -1;
		}

		// -------------------------------------------------------
		// find function index
		// -------------------------------------------------------
		int Script::function_index(const char* name) {
			char t[4];
			for (int i = 0; i < 3; ++i) {
				t[i] = name[i];
			}
			t[3] = '\0';
			for (int i = 0; i < NUM_FUNCTIONS; ++i) {
				if (strcmp(FUNCTIONS[i].name, t) == 0) {
					return i;
				}
			}
			return -1;
		}

		// -------------------------------------------------------
		// find function index
		// -------------------------------------------------------
		int Script::constant_index(const char* name) {
			StaticHash hash = SID(name);
			for (uint32_t i = 0; i < constants.size(); ++i) {
				if (constants[i].hash == hash) {
					return i;
				}
			}
			return -1;
		}

		// -------------------------------------------------------
		// parse operand
		// -------------------------------------------------------
		int Script::parseOperand(Variable* var, const char* name, int index) {
			int idx = index;
			int fi = function_index(name);
			if (fi != -1) {
				var->type = VT_FUNCTION;
				var->index = fi;
			}
			else {
				int ri = get_register(name);
				if (ri >= 0 && ri < 5) {
					var->type = VT_REG;
					var->index = ri;
				}
				else {
					int oi = constant_index(name);
					if (oi != -1) {
						var->type = VT_CONSTANT;
						var->index = oi;
					}
					else {
						var->type = VT_VARIABLE;
						int idx = -1;
						StaticHash hash(name);
						for (uint32_t i = 0; i < variables.size(); ++i) {
							if (variables[i].hash == hash) {
								idx = i;
							}
						}
						if (idx != -1) {
							var->index = idx;
						}
					}
				}
			}
			// can be variable or function
			++idx;
			return idx;
		}

		// -------------------------------------------------------
		// parse function
		// -------------------------------------------------------
		int Script::parseFunction(const char* data, const Tokenizer& t, int index, Variable* var) {
			int idx = index + 1;
			char name[128] = { '\0' };
			const FunctionDefinition& def = FUNCTIONS[var->index];
			Function f;
			f.type = def.type;
			f.arguments = def.arguments;
			f.function_index = var->index;
			for (int i = 0; i < def.arguments; ++i) {				
				const Token& tok = t.get(idx);				
				FunctionArgument& arg = f.args[i];
				if (tok.type == Token::NAME) {
					strncpy(name, data + tok.index, tok.size);
					int ri = get_register(name);
					if (ri > 0 && ri < 5) {
						arg.type = VT_REG;
						arg.index = ri;
					}
					else {
						int oi = constant_index(name);
						if (oi != -1) {
							arg.type = VT_CONSTANT;
							arg.index = oi;
						}
						else {
							arg.type = VT_VARIABLE;
							int idx = -1;
							StaticHash hash(name);
							for (uint32_t i = 0; i < variables.size(); ++i) {
								if (variables[i].hash == hash) {
									idx = i;
								}
							}
							if (idx != -1) {
								arg.index = idx;
							}
						}
					}
				}
				else {
					arg.type = VT_NUMBER;
					arg.index = add(v4(tok.value));
				}
				idx += 2;				
			}
			functions.push_back(f);
			var->index = functions.size() - 1;
			++idx;
			return idx;
		}

		// -------------------------------------------------------
		// parse line
		// -------------------------------------------------------
		int Script::parseLine(const char* data, Tokenizer& t, int index, Line* line) {
			char name[128] = { '\0' };
			Token& tok = t.get(index);
			strncpy(name, data + tok.index, tok.size);
			line->register_index = get_register(name);
			++index;
			++index; // ASSIGN
			tok = t.get(index);
			if (tok.type == Token::NAME) {
				strncpy(name, data + tok.index, tok.size);
				index = parseOperand(&line->var1, name, index);
				if (line->var1.type == VT_FUNCTION) {
					index = parseFunction(data, t, index, &line->var1);
				}
			}
			else if (tok.type == Token::NUMBER) {
				Variable v1;
				v1.type = VT_NUMBER;
				v1.index = add(v4(tok.value));
				line->var1 = v1;
				++index;
			}
			else {
				++index;
			}
			tok = t.get(index);
			// simple assignment
			if (tok.type == Token::SEMICOLON) {
				line->operation = OP_NONE;
				index += 2;
				return index;
			}
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
				index = parseOperand(&line->var2, name, index);
				if (line->var2.type == VT_FUNCTION) {
					index = parseFunction(data, t, index, &line->var2);
				}
			}
			else if (tok.type == Token::NUMBER) {
				Variable v;
				v.type = VT_NUMBER;
				v.index = add(v4(tok.value));
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

		bool isMethodDefinition(const char* data, const Tokenizer& t, int index) {
			Token tok = t.get(index);
			char name[32] = { '\0' };
			if (tok.type == Token::NAME) {
				strncpy(name, data + tok.index, tok.size);
				if (strcmp(name, "function") == 0) {
					return true;
				}
			}
			return false;
		}

		bool isClosing(const Tokenizer& t, int index) {
			Token tok = t.get(index);
			if (tok.type == Token::CLOSE_BRACES) {
				return true;
			}
			return false;
		}

		int Script::getMethod(const char* data, const Tokenizer& t, int index, Method* m) {
			int idx = index;
			// function wiggle() {
			char name[32] = { '\0' };
			const Token& tok = t.get(idx);
			if (tok.type == Token::NAME) {
				strncpy(name, data + tok.index, tok.size);
				m->hash = SID(name);
				idx += 3;
				const Token& l = t.get(idx);
				if (l.type == Token::OPEN_BRACES) {
					idx += 2;
				}
				else {
					// report error
				}
			}
			else {
				// report error
			}
			return idx;
		}
		// -------------------------------------------------------
		// parse
		// -------------------------------------------------------
		void Script::parse(const char* text) {
			Tokenizer t;
			t.parse(text);
			int n = 0;
			int idx = -1;
			if (isMethodDefinition(text, t, 0)) {
				while (n < t.size()) {
					if (isMethodDefinition(text, t, n)) {
						++n;
						Method m;
						n = getMethod(text, t, n, &m);
						_methods.push_back(m);
						idx = _methods.size() - 1;
					}
					else {
						if (isClosing(t, n)) {
							idx = -1;
							++n;
						}
						if (idx != -1) {
							Method& m = _methods[idx];
							Line line;
							n = parseLine(text, t, n, &line);
							m.lines.push_back(line);
						}
					}
				}
			}
			else {
				Method m;
				m.hash = SID("default");
				while (n < t.size()) {					
					Line line;
					n = parseLine(text, t, n, &line);
					m.lines.push_back(line);
				}
				_methods.push_back(m);
			}
		}

		// -------------------------------------------------------
		// execute function
		// -------------------------------------------------------
		v4 Script::executeFunction(const Function& f) {
			v4 ret(0, 0, 0, 0);
			v4 args[4];
			for (int i = 0; i < f.arguments; ++i) {
				const FunctionArgument& arg = f.args[i];
				if (arg.type == VT_CONSTANT) {
					args[i] = constants[arg.index].value;
				}
				else if (arg.type == VT_NUMBER) {
					args[i] = numbers[arg.index];
				}
				else if (arg.type == VT_REG) {
					args[i] = data[arg.index];
				}
				else if (arg.type == VT_VARIABLE) {
					args[i] = variables[arg.index].value;
				}
			}
			const FunctionDefinition& def = FUNCTIONS[f.function_index];
			ret = (*def.function)(args, f.arguments);
			return ret;
		}

		// -------------------------------------------------------
		// get data from context by variable
		// -------------------------------------------------------
		v4 Script::get_data(const Variable& var) {
			if (var.type == VT_CONSTANT) {
				return constants[var.index].value;
			}
			else if (var.type == VT_NUMBER) {
				return numbers[var.index];
			}
			else if (var.type == VT_REG) {
				return data[var.index];
			}
			else if (var.type == VT_VARIABLE) {
				return variables[var.index].value;
			}
			else if (var.type == VT_FUNCTION) {
				return executeFunction(functions[var.index]);
			}
			return v4(0, 0, 0, 0);
		}

		// -------------------------------------------------------
		// register variable
		// -------------------------------------------------------
		int Script::registerVar(StaticHash hash) {
			int idx = -1;
			for (uint32_t i = 0; i < variables.size(); ++i) {
				if (variables[i].hash == hash) {
					idx = i;
				}
			}
			if (idx == -1) {
				VMVariable c;
				c.hash = hash;
				c.value = v4(0.0f);
				variables.push_back(c);
				idx = variables.size() - 1;
			}
			return idx;
		}

		// -------------------------------------------------------
		// set variable by index
		// -------------------------------------------------------
		void Script::set(int idx, const v4& v) {
			if (idx >= 0 && idx < variables.size()) {
				variables[idx].value = v;
			}
		}

		// -------------------------------------------------------
		// set variable by hash
		// -------------------------------------------------------
		void Script::set(StaticHash hash, const v4& v) {
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

		// -------------------------------------------------------
		// add constant
		// -------------------------------------------------------
		int Script::addConstant(StaticHash hash,const v4& v) {
			VMVariable c;
			c.hash = hash;
			c.value = v;
			constants.push_back(c);
			return constants.size() - 1;
		}

		const Method UNKNOWN;

		const Method& Script::getMethod(StaticHash hash) const {
			for (uint32_t i = 0; i < _methods.size(); ++i) {
				if (_methods[i].hash == hash) {
					return _methods[i];
				}
			}
			return UNKNOWN;
		}
		// -------------------------------------------------------
		// execute
		// -------------------------------------------------------
		void Script::execute() {
			const Method& m = _methods[0];
			for (uint32_t i = 0; i < m.lines.size(); ++i) {
				const Line& l = m.lines[i];
				int oi = l.register_index;
				// simple assignment
				if (l.operation == Operation::OP_NONE) {
					Variable v = l.var1;
					if (v.type == VT_CONSTANT) {
						data[oi] = constants[v.index].value;
					}
					else if (v.type == VT_NUMBER) {
						data[oi] = numbers[v.index];
					}
					else if (v.type == VT_FUNCTION) {
						data[oi] = executeFunction(functions[v.index]);
					}
				}
				else {
					v4 t1 = get_data(l.var1);
					v4 t2 = get_data(l.var2);
					switch (l.operation) {
					case OP_PLUS: data[oi] = t1 + t2; break;
					case OP_MINUS: data[oi] = t1 - t2; break;
					case OP_DIV:
						for (int i = 0; i < 4; ++i) {
							data[oi].data[i] = t1.data[i] / t2.data[i];
						}
						break;
					case OP_MUL:
						for (int i = 0; i < 4; ++i) {
							data[oi].data[i] = t1.data[i] * t2.data[i];
						}
						break;
					}
				}
			}
		}

		// -------------------------------------------------------
		// load data
		// -------------------------------------------------------
		bool Script::loadData(const char* text) {
			parse(text);
			return true;
		}

		// -------------------------------------------------------
		// reload data
		// -------------------------------------------------------
		bool Script::reloadData(const char* text) {
			lines.clear();
			constants.clear();
			functions.clear();
			parse(text);
			return true;
		}

	}

}