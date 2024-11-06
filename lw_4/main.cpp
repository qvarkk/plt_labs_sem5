#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>

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

Func	: sin |   S
		  cos |   K
		  sqr |   Q
		  sqrt    R

Id		: Alpha Id |
		  Alpha

Const	: Digit Const |
		  Digit |
		  .Deci

Deci	: Digit Deci |
		  Digit |

Alpha	: a-z

Digit	: 0-9

∅
*/

class Interpreter {
public:
	Interpreter() {
		file = std::ifstream("input.txt");
		out_file = std::ofstream("output.txt");
		current_char = '\0';
	}

	~Interpreter() {
		file.close();
		out_file.close();
	}

	void interpret() {
		int i = 1;

		get_next_char();
		while (current_char != EOF) {
			int var = proc_id(true);

			if (current_char != '(')
				error("'(' Expected");
			get_next_char();

			int value = proc_expr();

			if (current_char != ')')
				error("')' Expected");
			get_next_char();

			triads.push_back(std::tuple<OpType, std::string, std::string>(OpType::Equals, "^" + std::to_string(var), "^" + std::to_string(value)));

			for (auto& x : symbol_table) {
				if (x.first == std::get<1>(triads.at(var - 1))) {
					x.second = process_triad(value - 1);
				}
			}

			i++;
		}

		output_triads();

		std::cout << symbol_table.size() << " variables defined:" << std::endl;
		for (auto x : symbol_table) {
			std::cout << x.first << " = " << x.second << std::endl;
		}
	}

private:
	enum OpType {
		Const,
		Variable,
		Equals,
		Plus,
		Minus,
		Sin,
		Cos,
		Sqr,
		Sqrt
	};

	std::vector<std::pair<std::string, double>> symbol_table;
	std::vector<std::tuple<OpType, std::string, std::string>> triads;
	char current_char;
	std::ifstream file;
	std::ofstream out_file;

	char get_op_char(OpType op_type) {
		switch (op_type) {
		case OpType::Const:
			return 'C';
		case OpType::Variable:
			return 'V';
		case OpType::Equals:
			return '=';
		case OpType::Plus:
			return '+';
		case OpType::Minus:
			return '-';
		case OpType::Sin:
			return 'S';
		case OpType::Cos:
			return 'K';
		case OpType::Sqr:
			return 'Q';
		case OpType::Sqrt:
			return 'R';
		}
	}

	void error(std::string message) {
		std::cout << "\nError: " << message << std::endl;

		output_triads();
		out_file << "Error: " << message << std::endl;

		throw std::exception();
	}

	void output_triads() {
		int i = 1;
		for (auto x : triads) {
			out_file << i << ". " << get_op_char(std::get<0>(x)) << "(" << std::get<1>(x) << ", " << std::get<2>(x) << ")\n";
			i++;
		}
	}

	bool isalpha(char c) {
		return c >= 'a' && c <= 'z';
	}

	bool isdigit(char c) {
		return c >= '0' && c <= '9';
	}

	bool isspace(char c) {
		return c == ' ' || c == '\t' || c == '\n' || c == '\r';
	}

	void get_next_char() {
		if (current_char != EOF) {
			current_char = file.get();

			while (isspace(current_char)) {
				current_char = file.get();
			}
		}
	}

	double process_triad(int pos) {
		OpType op = std::get<0>(triads.at(pos));
		std::string first = std::get<1>(triads.at(pos));
		std::string second = std::get<2>(triads.at(pos));

		if (first[0] == '^')
			first.erase(0, 1);

		if (second[0] == '^')
			second.erase(0, 1);

		switch (op) {
		case OpType::Equals:
			return process_triad(std::stoi(second) - 1);
		case OpType::Plus:
			return process_triad(std::stoi(first) - 1) + process_triad(std::stoi(second) - 1);
		case OpType::Minus:
			return process_triad(std::stoi(first) - 1) - process_triad(std::stoi(second) - 1);
		case OpType::Sin:
			return std::sin(process_triad(std::stoi(first) - 1));
		case OpType::Cos:
			return std::cos(process_triad(std::stoi(first) - 1));
		case OpType::Sqr:
			return std::pow(process_triad(std::stoi(first) - 1), 2);
		case OpType::Sqrt:
			return std::sqrt(process_triad(std::stoi(first) - 1));
		case OpType::Const:
			return std::stod(first);
		case OpType::Variable:
			for (auto x : symbol_table) {
				if (x.first == first)
					return x.second;
			}
		}
	}

	int proc_id(bool create = false) {

		std::string id = "";
		while (isalpha(current_char) && current_char != EOF) {
			id += current_char;
			get_next_char();
		}

		if (id == "sin" || id == "cos" || id == "sqr" || id == "sqrt") {
			if (!create) {
				return proc_func(id);
			}
			else {
				error(id + " is reserved name for a function");
			}
		}

		for (auto x : symbol_table) {
			if (x.first == id) {
				triads.push_back(std::tuple<OpType, std::string, std::string>(OpType::Variable, id, "∅"));
				return triads.size();
			}
		}

		if (create) {
			symbol_table.push_back(std::pair<std::string, double>(id, 0));
			triads.push_back(std::tuple<OpType, std::string, std::string>(OpType::Variable, id, "∅"));
			return triads.size();
		}
		else
			error("No variable with this name");
	}

	int proc_expr() {
		int value = proc_operand();

		while (current_char == '+' || current_char == '-') {
			char op = current_char;
			get_next_char();

			int next_operand = proc_operand();

			if (op == '+') {
				triads.push_back(std::tuple<OpType, std::string, std::string>(OpType::Plus, "^" + std::to_string(value), "^" + std::to_string(next_operand)));
				value = triads.size();
			}
			else if (op == '-') {
				triads.push_back(std::tuple<OpType, std::string, std::string>(OpType::Minus, "^" + std::to_string(value), "^" + std::to_string(next_operand)));
				value = triads.size();
			}
		}

		return triads.size();
	}

	int proc_operand() {
		if (isalpha(current_char))
			return proc_id();
		else if (isdigit(current_char))
			return proc_const();
		else
			error("Expected a digit or a number. Got " + current_char);
	}

	int proc_func(std::string func_name) {

		if (current_char != '(')
			error("'(' Expected");
		get_next_char();

		if (func_name == "sin") {
			triads.push_back(std::tuple<OpType, std::string, std::string>(OpType::Sin, "^" + std::to_string(proc_expr()), "∅"));
		}
		else if (func_name == "cos") {
			triads.push_back(std::tuple<OpType, std::string, std::string>(OpType::Cos, "^" + std::to_string(proc_expr()), "∅"));
		}
		else if (func_name == "sqr") {
			triads.push_back(std::tuple<OpType, std::string, std::string>(OpType::Sqr, "^" + std::to_string(proc_expr()), "∅"));
		}
		else if (func_name == "sqrt") {
			triads.push_back(std::tuple<OpType, std::string, std::string>(OpType::Sqrt, "^" + std::to_string(proc_expr()), "∅"));
		}
		else {
			error("Wrong function name");
		}

		if (current_char != ')')
			error("')' Expected");
		get_next_char();

		return triads.size();
	}

	int proc_const() {
		std::string result = "";

		while (isdigit(current_char) && current_char != EOF) {
			result += current_char;
			get_next_char();
		}
		
		if (current_char == '.') {
			get_next_char();
			triads.push_back(std::tuple<OpType, std::string, std::string>(OpType::Const, result + proc_deci(), "∅"));
		}
		else {
			triads.push_back(std::tuple<OpType, std::string, std::string>(OpType::Const, result, "∅"));
		}

		return triads.size();
	}

	std::string proc_deci() {
		std::string result = ".";

		while (isdigit(current_char) && current_char != EOF) {
			result += current_char;
			get_next_char();
		}

		return result;
	}
};

int main() {
	Interpreter i;
	i.interpret();
}