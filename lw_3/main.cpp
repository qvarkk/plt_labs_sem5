#include <iostream>
#include <fstream>
#include <vector>
#include <string>

/*
		GRAMMAR

Line	: Id(Expr)

Expr	: Expr + Operand |
		  Expr - Operand |
		  Operand

Operand : Value |
		  Func (Expr)

Value	: Id | 
		  Const

Func	: sin |
		  cos |
		  sqr |
		  sqrt

Id		: Alpha Id |
		  Alpha

Const	: Digit Const |
		  Digit |
		  .Deci

Deci	: Digit Deci |
		  Digit |

Alpha	: a-z

Digit	: 0-9

*/

class Interpreter {
public:
	Interpreter() {
		file = std::ifstream("input.txt");
		current_char = '\0';
	}

	~Interpreter() {
		file.close();
	}

	void interpret() {
		std::cout << "Begin parsing." << std::endl;
		int i = 1;

		while (current_char != EOF) {
			std::cout << "Operator " << i << ":\t";

			get_next_char();
			std::pair<std::string, double> var = proc_id(true);

			if (current_char != '(')
				error("'(' Expected");
			get_next_char();

			double value = proc_expr();

			if (current_char != ')')
				error("')' Expected");
			get_next_char();

			for (auto &x : symbol_table) {
				if (x.first == var.first) {
					x.second = value;
				}
			}

			if (current_char != '\n' && current_char != '\r' && current_char != EOF)
				error("Newline expected");

			i++;
		}

		std::cout << "\nEnd parsing." << std::endl;
		std::cout << symbol_table.size() << " variables defined:" << std::endl;
		for (auto x : symbol_table) {
			std::cout << x.first << " = " << x.second << std::endl;
		}
	}

private:
	std::vector<std::pair<std::string, double>> symbol_table;
	char current_char;
	std::ifstream file;

	void error(const char* message) {
		std::cout << "\nError: " << message << std::endl;
		throw std::exception(message);
	}

	bool isalpha(char c) {
		return c >= 'a' && c <= 'z';
	}

	bool isdigit(char c) {
		return c >= '0' && c <= '9';
	}

	bool isspace(char c) {
		return c == ' ' || c == '\t';
	}

	void peek_next_char() {
		if (current_char != EOF) {
			current_char = file.get();
		}
	}

	void get_next_char() {
		if (current_char != EOF) {
			current_char = file.get();

			while (isspace(current_char)) {
				current_char = file.get();
				std::cout << ' ';
			}

			std::cout << current_char;
		}
	}

	std::pair<std::string, double> proc_id(bool create = false) {
		std::streampos start_pos = file.tellg();
		start_pos -= 1;

		std::string id = "var";
		while (isalpha(current_char) && current_char != EOF) {
			id += current_char;
			get_next_char();
		}

		if (id == "sin" || id == "cos" || id == "sqr" || id == "sqrt") {
			if (!create) {
				file.seekg(start_pos);
				peek_next_char();
				return std::pair<std::string, double>(id, proc_func());
			}
			else {
				error("This name is reserved for a function");
			}
		}

		

		for (auto x : symbol_table) {
			if (x.first == id) {
				return x;
			}
		}

		if (create) {
			symbol_table.push_back(std::pair<std::string, double>(id, 0));
			return std::pair<std::string, double>(id, 0);
		}
		else
			error("No variable with this name");
		
	}

	double proc_expr() {
		double value = proc_operand();

		while (current_char == '+' || current_char == '-') {
			char op = current_char;
			get_next_char();

			double next_operand = proc_operand();

			if (op == '+') {
				value += next_operand;
			}
			else if (op == '-') {
				value -= next_operand;
			}
		}
		
		return value;
	}

	double proc_operand() {
		if (isalpha(current_char)) {
			return proc_id().second;
		}
		else if (isdigit(current_char))
			return proc_value();
	}

	double proc_func() {
		std::string func_name = "";

		while (isalpha(current_char) && current_char != EOF) {
			func_name += current_char;
			peek_next_char();
		}

		if (current_char != '(')
			error("'(' Expected");
		get_next_char();

		double res = 0;
		if (func_name == "sin") {
			res = std::sin(proc_expr());
		}
		else if (func_name == "cos") {
			res = std::cos(proc_expr());
		}
		else if (func_name == "sqr") {
			res = std::pow(proc_expr(), 2);
		}
		else if (func_name == "sqrt") {
			res = std::sqrt(proc_expr());
		}
		else {
			error("Wrong function name");
		}

		if (current_char != ')')
			error("')' Expected");
		get_next_char();

		return res;
	}

	double proc_value() {
		if (isalpha(current_char))
			return proc_id().second;
		else if (isdigit(current_char))
			return proc_const();
	}

	double proc_const() {
		std::string result = "";

		while (isdigit(current_char) && current_char != EOF) {
			result += current_char;
			get_next_char();
		}

		if (current_char == '.') {
			get_next_char();
			return std::stod(result) + proc_deci();
		}
		else
			return std::stod(result);
	}

	double proc_deci() {
		std::string result = "0.";

		while (isdigit(current_char) && current_char != EOF) {
			result += current_char;
			get_next_char();
		}

		return std::stod(result);
	}
};

int main() {
	Interpreter i;
	i.interpret();
}