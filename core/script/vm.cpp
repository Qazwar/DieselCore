#include "vm.h"
#include <stdio.h>
#include "..\io\json.h"
#include <stdint.h>

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

		const char* FUNCTIONS[] = { "SIN", "COS" };

		struct Variable {
			VarType type;
		};

		struct Line {
			int register_index;
			Operation operation;
			Variable var1;
			Variable var2;
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
				if (strcmp(FUNCTIONS[i], t) == 0) {
					return i;
				}
			}
			return -1;
		}

		int parseLine(const char* data,Tokenizer& t, int index, Line* line) {
			char name[128] = { '\0' };
			Token& tok = t.get(index);
			strncpy(name, data + tok.index, tok.size);
			line->register_index = get_register(name);
			++index;
			++index; // ASSIGN
			tok = t.get(index);
			if (tok.type == Token::NAME) {
				strncpy(name, data + tok.index, tok.size);
				int fi = function_index(name);
				if (fi != -1) {
					printf("FOUND FUNCTION!!!\n");
					line->var1.type = VT_FUNCTION;
					tok = t.get(++index); // OPEN BRACKETS
					tok = t.get(++index); // OPEN BRACKETS
					tok = t.get(++index); // CLOSE BRACKETS

				}
				else {
					int ri = get_register(name);
					if (ri >= 0 && ri < 5) {
						line->var1.type = VT_REG;
					}
					else {
						line->var1.type = VT_VARIABLE;
					}
				}
				// can be variable or function
				++index;
			}
			else if (tok.type == Token::NUMBER) {
				Variable v1;
				v1.type = VT_CONSTANT;
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
				int fi = function_index(name);
				if (fi != -1) {
					printf("FOUND FUNCTION!!!\n");
					line->var1.type = VT_FUNCTION;
					tok = t.get(++index); // OPEN BRACKETS
					tok = t.get(++index); // OPEN BRACKETS
					tok = t.get(++index); // CLOSE BRACKETS

				}
				else {
					int ri = get_register(name);
					if (ri >= 0 && ri < 5) {
						line->var2.type = VT_REG;
					}
					else {
						line->var2.type = VT_VARIABLE;
					}
				}
				// can be variable or function
				++index;
			}
			else if (tok.type == Token::NUMBER) {
				Variable v;
				v.type = VT_CONSTANT;
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

		void parse(const char* text) {
			Tokenizer t;
			//printf("%s", text);
			t.parse(text);
			int n = 0;
			for (int i = 0; i < t.size(); ++i) {
				const Token& tok = t.get(i);
				printf("%d = %d %s\n", i, tok.type, t.name(i));
			}
			Array<Line> lines;
			while (n < t.size()) {
				Line line;
				n = parseLine(text, t, n, &line);
				lines.push_back(line);
			}
			printf("lines: %d\n", lines.size());
			for (int i = 0; i < lines.size(); ++i) {
				const Line& l = lines[i];
				printf("%d : REG: %d OP: %d Type1: %d Type2: %d\n", i, l.register_index,l.operation,l.var1.type,l.var2.type);
			}
		}

	}

}