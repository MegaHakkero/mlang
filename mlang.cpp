#include <medjed/types.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <cstdio>
#include <string>
#include <cmath>
#include <stack>

extern "C" {
#include <unistd.h>
}

// Reverse Polish language, copyleft Medjed 2018

/*
 * Usage: mlang [expression]
 *   If [expression] isn't provided, the standard input will be used to
 *   read them.
 *
 * The other operators are self-explanatory, here's a list of the weird ones
 * 't'     - copy the value at the top of the stack
 *         - ex. "1tt++" = 3
 * 'print' - print the stack top of the given stack
 *         - parameters: <value> <int stack> - This can be either 0 or 1; 0
 *         - prints from the number stack, 1 prints from the string stack
 */

#define RPO_STR_DELIM '"'
#define RPO_STR_ESC   '\\'
#define RPO_OP_START  '$'

#define DO_DEBUG 0
#define DEBUG if (DO_DEBUG)

std::stack<I64> int_stack;
std::stack<std::string> str_stack;
std::string ops = " t^*/%+-";

U0 push_int(I64 n, bool negate = false) {
	DEBUG printf("push '%ld'\n", n);
	int_stack.push((negate) ? -n : n);
}

U0 push_str(std::string s) {
	DEBUG printf("push '%s'\n", s.c_str());
	str_stack.push(s);
}

template <class RT>
RT retpop(std::stack<RT>& stk) {
	RT v = stk.top();
	DEBUG std::cout << "pop '" << v << "'" << std::endl;
	stk.pop();
	return v;
}

bool is_negate_at(std::string& str, U64 i) {
	if (i + 1 >= str.size() - 1) return false;
	if (str[i] == '-' && isdigit(str[i + 1]))
		return true;
	return false;
}

bool is_seq_end_at(std::string& str, U64 i, bool is_op) {
	if (is_op) {
		if (str[i] == ' ' || i + 1 > str.size() - 1)
			return true;
	} else {
		if (i - 1 < 0) return false;
		if (str[i] == RPO_STR_DELIM && str[i - 1] != RPO_STR_ESC)
			return true;
	}
	return false;
}

C8 to_esc(C8& c) {
	switch(c) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		default:  return c;
	}
}

bool is_op(C8& c) {
	if (ops.find(c) != std::string::npos)
		return true;
	return false;
}

bool do_power() {
	if (int_stack.size() < 2) {
		printf("Syntax error - '^' requires two numbers\n");
		return false;
	}
	I64 n2 = retpop<I64>(int_stack);
	I64 n1 = retpop<I64>(int_stack);
	push_int(pow(n1, n2));
	return true;
}

bool do_multi() {
	if (int_stack.size() < 2) {
		printf("Syntax error - '*' requires two numbers\n");
		return false;
	}
	I64 n2 = retpop<I64>(int_stack);
	I64 n1 = retpop<I64>(int_stack);
	push_int(n1 * n2);
	return true;
}

bool do_div() {
	if (int_stack.size() < 2) {
		printf("Syntax error - '/' requires two numbers\n");
		return false;
	}
	I64 n2 = retpop<I64>(int_stack);
	I64 n1 = retpop<I64>(int_stack);
	if (!n2) {
		printf("Error - Can't divide by 0\n");
		return false;
	}
	push_int(n1 / n2);
	return true;
}

bool do_mod() {
	if (int_stack.size() < 2) {
		printf("Error - '%%' requires two numbers\n");
		return false;
	}
	I64 n2 = retpop<I64>(int_stack);
	I64 n1 = retpop<I64>(int_stack);
	push_int(n1 % n2);
	return true;
}

bool do_add() {
	if (int_stack.size() < 2) {
		printf("Syntax error - '+' requires two numbers\n");
		return false;
	}
	I64 n2 = retpop<I64>(int_stack);
	I64 n1 = retpop<I64>(int_stack);
	push_int(n1 + n2);
	return true;
}

bool do_sub() {
	if (int_stack.size() < 2) {
		printf("Syntax error - '-' requires two numbers\n");
		return false;
	}
	I64 n2 = retpop<I64>(int_stack);
	I64 n1 = retpop<I64>(int_stack);
	push_int(n1 - n2);
	return true;
}

bool do_copy() {
	if (!int_stack.size()) {
		printf("Error - 't' requires one value in the stack\n");
		return false;
	}
	push_int(int_stack.top());
	return true;
}

bool do_print() {
	if (!int_stack.size()) {
		printf("Syntax error - 'print' requires a number\n");
		return false;
	}
	I64 stk = retpop<I64>(int_stack);
	if (!stk) {     // 0 = number stack
		if (int_stack.size())
			printf("%ld", int_stack.top());
		return true;
	} else
	if (stk == 1) { // 1 = string stack
		if (str_stack.size())
			printf("%s", str_stack.top().c_str());
		return true;
	} else {
		printf("Error - invalid stack for 'print': %ld\n", stk);
		return false;
	}
}

bool op_unknown(std::string& op) {
	printf("Unknown operator '%s'\n", op.c_str());
	return false;
}

template <class S>
bool do_op(S& s) {
	std::string op;
	op += s;
	DEBUG printf("operator '%s'\n", op.c_str());
	if (op == " ")
		return true;
	else
	if (op == "t")
		return do_copy();
	else
	if (op == "^")
		return do_power();
	else
	if (op == "*")
		return do_multi();
	else
	if (op == "/")
		return do_div();
	else
	if (op == "%")
		return do_mod();
	else
	if (op == "+")
		return do_add();
	else
	if (op == "-")
		return do_sub();
	else
	if (op == "print")
		return do_print();
	else
	if (op == "exit")
		exit(0);
	else
		return op_unknown(op);
	return false;
}

bool rpo_line(std::string& str) {
	if (!str[0])
		return false;
	std::string curnum;
	std::string curstr;
	bool negate = false;
	bool in_str = false; // a string is being parsed
	bool in_op  = false; // an operator is being parsed
	for (U64 i = 0; i < str.size(); i++) {
		C8& c = str[i];
		if (in_str || in_op) {
			// we are in a character sequence - parse it.
			if (is_seq_end_at(str, i, in_op)) {
				if (i + 1 > str.size() - 1)
					curstr += str.back();
				if (in_op) {
					// execute the operator
					if (!do_op<std::string>(curstr))
						return false;
					in_op = false;
					curstr = "";
					continue;
				}
				push_str(curstr);
				curstr = "";
				in_str = false;
				continue;
			}
			if (in_str) {
				if (c == RPO_STR_ESC) continue; // ignore escapes directly
				if (str[i - 1] == RPO_STR_ESC)
					// and parse only when they precede the current token
					curstr += to_esc(c);
				else
					curstr += c;
				continue;
			}
			curstr += c;
			continue;
		}
		if (c == RPO_STR_DELIM) {
			// start parsing string - ignores escapes
			if (curnum[0]) {
				DEBUG printf("line 257:\n");
				push_int(std::stoll(curnum), negate);
				if (negate)
					negate = false;
				curnum = "";
			}
			in_str = true;
			continue;
		} else
		if (c == RPO_OP_START) {
			// start parsing operator - also ignores escapes
			if (curnum[0]) {
				DEBUG printf("line 268:\n");
				push_int(std::stoll(curnum), negate);
				if (negate)
					negate = false;
				curnum = "";
			}
			in_op = true;
			continue;
		}
		if (is_op(c)) {
			if (curnum[0]) {
				// push number to stack when an operator is read
				DEBUG printf("line 279:\n");
				push_int(std::stoll(curnum), negate);
				if (negate)
					negate = false;
			}
			if (is_negate_at(str, i)) {
				negate = true;
				continue;
			}
			if (!do_op<C8>(c))
				return false;
			curnum = "";
			continue;
		}
		if (!isdigit(c)) {
			printf("Invalid character: '%c'\n", c);
			return false;
		}
		curnum += c;
	}
	if (curnum[0]) {
		DEBUG printf("line 300\n");
		push_int(std::stoll(curnum), negate);
	}
	return true;
}

U0 exec_input(std::istream& s, std::string& in, bool prompt = false) {
	bool firstln = true;
	if (prompt) printf("> ");
	while (std::getline(s, in)) {
		if (firstln) {
			firstln = false;
			if (!in.find("#!"))
				continue;
		}
		if (!in[0]) goto label_prompt;
		rpo_line(in);
label_prompt:
		if (prompt) printf("> ");
	}	
}

I32 main(I32 argc, C8 **argv) {
	C8 c = 0;
	std::string in;
	while ((c = getopt(argc, argv, "he:")) != -1) {
		switch(c) {
			case 'e':
				in = optarg;
				continue;
			case 'h':
				printf("Usage: %s [-e] [file]\n", argv[0]);
				continue;
			default:
				printf("Unknown option: '-%c'\n", optopt);
				return 1;
		}
	}
	if (in[0]) {
		if (!rpo_line(in))
			return 1;
		if (int_stack.size() > 1)
			printf("Warning - Dangling values at stack top\n");
			// the above is only a warning in non-REPL mode
		return 0;
	}
	if (optind < argc)
		in = argv[optind];
	if (in[0]) {
		std::ifstream inf;
		inf.open(in);
		in = "";
		exec_input(inf, in);
		inf.close();
		return 0;
	}
	exec_input(std::cin, in, true);
	putchar('\n');
	return 0;
}
