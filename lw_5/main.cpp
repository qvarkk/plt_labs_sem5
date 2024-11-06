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

		out_file << "Unoptimized code:\n";
		output_triads();

		optimize_triads();
		out_file << "\n\nOptimized code:\n";
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
		Sqrt,
		NoOp
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
		case OpType::NoOp:
			return ' ';
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
			if (std::get<0>(x) != OpType::NoOp)
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

	bool isnumber(std::string str) {
		for (char ch : str) {
			if (!isdigit(ch) && ch != '.')
				return false;
		}
		return true;
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
	std::string double_to_str(double dbl) {
		std::string str = std::to_string(dbl);
		str.erase(str.find_last_not_of('0') + 1, std::string::npos);
		str.erase(str.find_last_not_of('.') + 1, std::string::npos);
		return str;
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
			return process_triad(std::stod(second) - 1);
		case OpType::Plus:
			return process_triad(std::stod(first) - 1) + process_triad(std::stod(second) - 1);
		case OpType::Minus:
			return process_triad(std::stod(first) - 1) - process_triad(std::stod(second) - 1);
		case OpType::Sin:
			return std::sin(process_triad(std::stod(first) - 1));
		case OpType::Cos:
			return std::cos(process_triad(std::stod(first) - 1));
		case OpType::Sqr:
			return std::pow(process_triad(std::stod(first) - 1), 2);
		case OpType::Sqrt:
			return std::sqrt(process_triad(std::stod(first) - 1));
		case OpType::Const:
			return std::stod(first);
		case OpType::Variable:
			for (auto x : symbol_table) {
				if (x.first == first)
					return x.second;
			}
		}
	}

	void optimize_triads() {
		for (auto& triad : triads) {
			OpType op = std::get<0>(triad);
			std::string first = std::get<1>(triad);
			std::string second = std::get<2>(triad);

			bool firstAddr = false;
			if (first[0] == '^') {
				first.erase(0, 1);
				firstAddr = true;
			}

			bool secondAddr = false;
			if (second[0] == '^') {
				second.erase(0, 1);
				secondAddr = true;
			}

			// Rule 1 (unary operation)
			if (firstAddr && second == "∅") {
				std::tuple<OpType, std::string, std::string>& addrTriad = triads.at(std::stoi(first) - 1);
				if (std::get<0>(addrTriad) == OpType::Const) {
					std::string val = std::get<1>(addrTriad);
					std::get<1>(triad) = val;
					std::get<0>(addrTriad) = OpType::NoOp;
					firstAddr = false;
					first = val;
				}
			}

			// Rule 1 (binary operation)
			if (secondAddr && op != OpType::Equals) {
				std::tuple<OpType, std::string, std::string>& addrTriad = triads.at(std::stoi(second) - 1);
				if (std::get<0>(addrTriad) == OpType::Const) {
					std::string val = std::get<1>(addrTriad);
					std::get<2>(triad) = val;
					std::get<0>(addrTriad) = OpType::NoOp;
					secondAddr = false;
					second = val;
				}
			}

			// Rule 2
			if (op != OpType::Const && !firstAddr && isnumber(first) && second == "∅") {
				double res = 0;

				switch (op) {
				case OpType::Sin:
					res = std::sin(std::stod(first));
					break;
				case OpType::Cos:
					res = std::cos(std::stod(first));
					break;
				case OpType::Sqr:
					res = std::pow(std::stod(first), 2);
					break;
				case OpType::Sqrt:
					res = std::sqrt(std::stod(first));
					break;
				}

				std::get<0>(triad) = OpType::Const;
				std::get<1>(triad) = double_to_str(res);

				op = OpType::Const;
				first = std::get<1>(triad);
			}

			// Rule 3
			if (op != OpType::Equals && firstAddr && !secondAddr && isnumber(second)) {
				std::tuple<OpType, std::string, std::string>& addrTriad = triads.at(std::stoi(first) - 1);
				if (std::get<0>(addrTriad) == OpType::Const) {
					double res = 0;

					switch (op) {
					case OpType::Plus:
						res = std::stod(std::get<1>(addrTriad)) + std::stod(first);
						break;
					case OpType::Minus:
						res = std::stod(std::get<1>(addrTriad)) - std::stod(first);
						break;
					}

					std::get<0>(addrTriad) = OpType::NoOp;
					std::get<0>(triad) = OpType::Const;
					std::get<1>(triad) = double_to_str(res);
					std::get<2>(triad) = "∅";

					firstAddr = false;
					op = OpType::Const;
					first = std::get<1>(triad);
				}
			}

			// Rule 4
			if (op == OpType::Equals) {
				std::tuple<OpType, std::string, std::string>& addrTriad = triads.at(std::stoi(first) - 1);

				std::string var_name = std::get<1>(addrTriad);
				std::get<0>(addrTriad) = OpType::NoOp;
				std::get<1>(triad) = var_name;
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