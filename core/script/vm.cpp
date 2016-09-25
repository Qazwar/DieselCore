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
		// vm clamp
		// -------------------------------------------------------
		v4 vm_clamp(v4* args, int num) {
			v4 ret(0.0f);
			v4 mn = args[1];
			v4 mx = args[2];
			for (int i = 0; i < 4; ++i) {
				ret.data[i] = args[0].data[i];
				if (ret.data[i] < mn.data[i]) {
					ret.data[i] = mn.data[i];
				}
				if (ret.data[i] > mx.data[i]) {
					ret.data[i] = mx.data[i];
				}
			}
			return ret;
		}

		// -------------------------------------------------------
		// vm saturate
		// -------------------------------------------------------
		v4 vm_saturate(v4* args, int num) {
			v4 ret(0.0f);
			for (int i = 0; i < 4; ++i) {
				ret.data[i] = args[0].data[i];
				if (ret.data[i] < 0.0f) {
					ret.data[i] = 0.0f;
				}
				if (ret.data[i] > 1.0f) {
					ret.data[i] = 1.0f;
				}
			}
			return ret;
		}

		// -------------------------------------------------------
		// vm tweening
		// -------------------------------------------------------
		v4 vm_tweening(v4* args, int num) {
			v4 type = args[0];
			tweening::TweeningType t = tweening::get_by_index(type.x);
			return tweening::interpolate(t, args[1], args[2], args[3].x, args[4].x);
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
		// translate variable type
		// -------------------------------------------------------
		const char* translate(VarType type) {
			switch (type) {
				case VT_REG: return "REG"; break;
				case VT_CONSTANT: return "CONSTANT"; break;
				case VT_FUNCTION: return "FUNCTION"; break;
				case VT_VARIABLE: return "VARIABLE"; break;
				case VT_NUMBER: return "NUMBER"; break;
				case VT_NONE: return "NONE"; break;
				default: return "?????";
			}
		}

		// -------------------------------------------------------
		// translate operation 
		// -------------------------------------------------------
		const char* translate(Operation op) {
			switch (op) {
				case OP_PLUS: return "+"; break;
				case OP_MINUS: return "-"; break;
				case OP_MUL: return "*"; break;
				case OP_DIV: return "/"; break;
				case OP_NONE: return "="; break;
				default: return "?";
			}
		}
		// -------------------------------------------------------
		// all functions
		// -------------------------------------------------------
		FunctionDefinition FUNCTIONS[] = {
			{ FT_SIN, "SIN", 1, &vm_sin},
			{ FT_COS, "COS", 1, &vm_cos},
			{ FT_LRP, "LRP", 3, &vm_lerp},
			{ FT_LRP, "LRP", 3, &vm_lerp },
			{ FT_SAT, "SAT", 1, &vm_saturate },
			{ FT_CLM, "CLM", 3, &vm_clamp },
			{ FT_TWN, "TWN", 5, &vm_tweening },
			{ FT_D2R, "D2R", 1, &vm_degtorad }
		};

		const int NUM_FUNCTIONS = 8;

		// -------------------------------------------------------
		// Script
		// -------------------------------------------------------

		// -------------------------------------------------------
		// extract register
		// -------------------------------------------------------
		int Script::get_register(const char* t) {
			// OUT is special case and it is mapped to 10
			if (t[0] == 'O' && t[1] == 'U' && t[2] == 'T') {
				return 10;
			}
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
				if (ri >= 0 && ri < 6) {
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
					if (ri >= 0 && ri < 6) {
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
			const Token& tok = t.get(idx);
			//++idx;
			return idx;
		}

		const Token::TokenType V4_DEF[] = { Token::NUMBER, Token::DELIMITER, Token::NUMBER, Token::DELIMITER, Token::NUMBER, Token::DELIMITER, Token::NUMBER, Token::CLOSE_BRACKET };

		// -------------------------------------------------------
		// parse number
		// -------------------------------------------------------
		int Script::parseNumber(const Tokenizer& t, int index, Variable* var) {
			int idx = index;
			const Token& tok = t.get(idx);
			if (tok.type == Token::OPEN_BRACKET) {
				++idx;
				for (int i = 0; i < 8; ++i) {
					const Token& tc = t.get(idx + i);
					if (tc.type != V4_DEF[i]) {
						LOGE << "Syntax error. Wrong v4 number definition";
						return -1;
					}
				}
				var->type = VT_NUMBER;
				v4 r(0.0f);
				for (int i = 0; i < 4; ++i) {
					const Token& nn = t.get(idx);
					r.data[i] = nn.value;
					idx += 2;
				}
				var->index = add(r);
			}
			return idx;
		}

		// -------------------------------------------------------
		// parse operand
		// -------------------------------------------------------
		int Script::parseOperand(const char* data, const Tokenizer& t,int index,Variable* var) {
			int idx = index;
			char name[128] = { '\0' };
			const Token& tok = t.get(idx);
			if (tok.type == Token::NAME) {
				strncpy(name, data + tok.index, tok.size);
				idx = parseOperand(var, name, idx);
				if (var->type == VT_FUNCTION) {
					idx = parseFunction(data, t, idx, var);
				}
			}
			else if (tok.type == Token::NUMBER) {
				var->type = VT_NUMBER;
				var->index = add(v4(tok.value));
				//line->var1 = v1;
				++idx;
			}
			else if (tok.type == Token::OPEN_BRACKET) {
				idx = parseNumber(t, index, var);
				if (idx == -1) {
					return -1;
				}
			}
			else {
				++idx;
			}
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
			index = parseOperand(data, t, index, &line->var1);
			tok = t.get(index);
			// simple assignment
			if (tok.type == Token::SEMICOLON) {
				line->operation = OP_NONE;
				line->var2.type = VT_NONE;
				line->var2.index = -1;
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
			index = parseOperand(data, t, index, &line->var2);
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

		const Token::TokenType METHOD_DEFS[] = { Token::NAME,Token::NAME,Token::OPEN_BRACKET,Token::CLOSE_BRACKET,Token::OPEN_BRACES };
		// -------------------------------------------------------
		// is method definition
		// -------------------------------------------------------
		bool isMethodDefinition(const char* data, const Tokenizer& t, int index) {
			Token tok = t.get(index);
			char name[32] = { '\0' };
			if (tok.type == Token::NAME) {
				strncpy(name, data + tok.index, tok.size);
				if (strcmp(name, "function") == 0) {
					for (int i = 0; i < 5; ++i) {
						const Token& current = t.get(index + i);
						if (current.type != METHOD_DEFS[i]) {
							return false;
						}
					}
					return true;
				}
			}
			return false;
		}

		// -------------------------------------------------------
		// is closing
		// -------------------------------------------------------
		bool isClosing(const Tokenizer& t, int index) {
			Token tok = t.get(index);
			if (tok.type == Token::CLOSE_BRACES) {
				return true;
			}
			return false;
		}

		// -------------------------------------------------------
		// get method
		// -------------------------------------------------------
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
							n += 2;
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
			v4 args[6];
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

		// -------------------------------------------------------
		// get method
		// -------------------------------------------------------
		const Method& Script::getMethod(StaticHash hash) const {
			for (uint32_t i = 0; i < _methods.size(); ++i) {
				if (_methods[i].hash == hash) {
					return _methods[i];
				}
			}
			return UNKNOWN;
		}

		// -------------------------------------------------------
		// debug method
		// -------------------------------------------------------
		void Script::debugMethod(StaticHash hash) {
			const Method& m = getMethod(hash);
			LOG << "----- Method ------";
			for (uint32_t i = 0; i < m.lines.size(); ++i) {
				const Line& l = m.lines[i];
				LOG << i << " => Reg: " << l.register_index << " Op: " << translate(l.operation) << " Var1: " << translate(l.var1.type) << " (" << l.var1.index << ") Var2: " << translate(l.var2.type) << " (" << l.var2.index << ")";
			}
		}
		// -------------------------------------------------------
		// execute default method
		// -------------------------------------------------------
		v4 Script::execute() {
			const Method& m = _methods[0];
			return execute(m);
		}

		// -------------------------------------------------------
		// execute method
		// -------------------------------------------------------
		v4 Script::execute(StaticHash hash) {
			for (uint32_t i = 0; i < _methods.size(); ++i) {
				const Method& m = _methods[i];
				if (m.hash == hash) {
					return execute(m);
				}
			}
			return v4(0.0f);
		}

		// -------------------------------------------------------
		// execute method by index
		// -------------------------------------------------------
		v4 Script::execute(uint32_t idx) {
			const Method& m = _methods[idx];
			return execute(m);
		}

		// -------------------------------------------------------
		// execute operation
		// -------------------------------------------------------
		void executeOperation(const v4& t1, const v4& t2, Operation op, v4* ret) {
			switch (op) {
				case OP_PLUS: *ret = t1 + t2; break;
				case OP_MINUS: *ret = t1 - t2; break;
				case OP_DIV:
					for (int i = 0; i < 4; ++i) {
						ret->data[i] = t1.data[i] / t2.data[i];
					}
					break;
				case OP_MUL:
					for (int i = 0; i < 4; ++i) {
						ret->data[i] = t1.data[i] * t2.data[i];
					}
					break;
			}
		}

		// -------------------------------------------------------
		// execute specific method
		// -------------------------------------------------------
		v4 Script::execute(const Method& m) {			
			v4 ret(0.0f);
			for (uint32_t i = 0; i < m.lines.size(); ++i) {
				const Line& l = m.lines[i];
				int oi = l.register_index;
				// simple assignment
				if (l.operation == Operation::OP_NONE) {
					Variable v = l.var1;
					if (oi == 10) {
						if (v.type == VT_CONSTANT) {
							ret = constants[v.index].value;
						}
						else if (v.type == VT_NUMBER) {
							ret = numbers[v.index];
						}
						else if (v.type == VT_FUNCTION) {
							ret = executeFunction(functions[v.index]);
						}
					}
					else {
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
				}
				else {
					v4 t1 = get_data(l.var1);
					v4 t2 = get_data(l.var2);
					// 10 = OUT
					if (oi == 10) {
						executeOperation(t1, t2, l.operation, &ret);
					}
					else {
						executeOperation(t1, t2, l.operation, &data[oi]);
					}
				}
			}
			return ret;
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
			_methods.clear();
			numbers.clear();
			functions.clear();
			parse(text);
			return true;
		}

	}

}