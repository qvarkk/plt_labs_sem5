#include "lexer.cpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <map>

class Interpreter {
private:
	std::ofstream output_file;
	Lexer& lexer;

	std::stack<char> stack;
	std::vector<std::pair<char, char>> equal_matrix; // = in stack
	std::vector<std::pair<char, char>> less_matrix; // < in stack
	std::vector<std::pair<char, char>> less_or_equal_matrix; // @ in stack
	std::vector<std::pair<char, char>> more_matrix; // < in stack
	std::map<char, std::vector<std::string>> grammar;

public:
	Interpreter(Lexer& lexer) : lexer(lexer) {
		output_file = std::ofstream("out.txt");
		init_matrices();
		init_grammar();
	}

	~Interpreter() {
		output_file.close();
	}

	void interpret() {
		std::string input_string = "";
		Token token = lexer.get_next_token();
		while (token.get_type() != END) {
			char char_to_add = token.get_type();
			if (char_to_add == Type::TERMINAL)
				char_to_add = token.get_value()[0];
			input_string.push_back(char_to_add);
			token = lexer.get_next_token();
		}
		input_string.push_back('?');
		
		stack.push('?');
		for (int i = 0; i < input_string.size(); i++) {
			char lexem = input_string[i];
			if (has_relation_in_matrix(stack.top(), lexem, less_or_equal_matrix)) {
				stack.push('@');
				stack.push(lexem);
				log_stack_state("shift (less or equal)");
			}
			else if (has_relation_in_matrix(stack.top(), lexem, equal_matrix)) {
				stack.push('=');
				stack.push(lexem);
				log_stack_state("shift (equal)");
			}
			else if (has_relation_in_matrix(stack.top(), lexem, less_matrix)) {
				stack.push('<');
				stack.push(lexem);
				log_stack_state("shift (less)");
			}
			else if (has_relation_in_matrix(stack.top(), lexem, more_matrix)) {
				reduce_stack();
				log_stack_state("reduce");
				i--;
			}
			else {
				error("invalid syntax (no such sequence of characters)");
			}
		}
		stack.push('?');
	}

private:
	void init_matrices() {
		equal_matrix = {
			{'L', '?'}, {'L', 'S'},
			{'E', '+'}, {'E', '-'}, 
			{'I', '('}, {'F', '('}, {'E', ')'},
		};

		less_matrix = {
			{'?', 'S'}, {'?', 'I'}, {'L', 'I'},
			{'+', 'I'}, {'+', 'C'}, {'+', 'F'},
			{'-', 'I'}, {'-', 'C'}, {'-', 'F'},
			{'(', 'T'}, {'(', 'I'}, {'(', 'C'}, {'(', 'F'},
		};

		less_or_equal_matrix = {
			{'(', 'E'}, {'+', 'T'},
			{'-', 'T'}, {'?', 'L'}
		};

		more_matrix = {
			{'S', '?'}, {')', '?'},
			{'S', 'S'}, {'S', 'I'}, {')', 'S'}, {')', 'I'},
			{'T', ')'}, {'I', ')'}, {'C', ')'}, {')', ')'},
			{'T', '+'}, {'I', '+'}, {'C', '+'}, {')', '+'},
			{'T', '-'}, {'I', '-'}, {'C', '-'}, {')', '-'},
		};
	}

	void init_grammar() {
		grammar = {
			{'L', {"LS", "S"}},
			{'S', {"I(E)"}},
			{'E', {"E+T", "E-T", "T"}},
			{'T', {"I", "C", "F(E)"}},
		};
	}

	void reduce_stack() {
		std::vector<char> popped;
		while (!stack.empty() && stack.top() != '<' && stack.top() != '@') {
			if (stack.top() != '<' && stack.top() != '=' && stack.top() != '@')
				popped.push_back(stack.top());
			stack.pop();
		}
		stack.pop();

		std::reverse(popped.begin(), popped.end());
		std::string popped_str(popped.begin(), popped.end());

		bool reduced = false;
		char char_to_push;
		for (auto& rules : grammar) {
			for (auto& rule : rules.second) {
				if (rule == popped_str) {
					char_to_push = rules.first;
					reduced = true;
					break;
				}
			}
			if (reduced)
				break;
		}

		if (!reduced)
			error("invalid syntax (no such rule)");

		if (has_relation_in_matrix(stack.top(), char_to_push, less_or_equal_matrix)) {
			stack.push('@');
		}
		else if (has_relation_in_matrix(stack.top(), char_to_push, equal_matrix)) {
			stack.push('=');
		}
		else if (has_relation_in_matrix(stack.top(), char_to_push, less_matrix)) {
			stack.push('<');
		}

		stack.push(char_to_push);
	}

	bool has_relation_in_matrix(char left, char right, std::vector<std::pair<char, char>> matrix) {
		for (auto key_value_pairs : matrix) {
			if (key_value_pairs.first == left && key_value_pairs.second == right)
				return true;
		}

		return false;
	}

	void log_stack_state(std::string message) {
		std::cout << "Action: " << message << std::endl;
		std::cout << "Stack: \n\t";

		std::stack<char> tmp = stack;
		std::vector<char> stack_chars;
		while (!tmp.empty()) {
			stack_chars.push_back(tmp.top());
			tmp.pop();
		}

		std::reverse(stack_chars.begin(), stack_chars.end());
		for (auto stack_element : stack_chars) {
			std::cout << stack_element;
		}

		std::cout << std::endl;
	}

	void error(std::string message) {
		std::cout << "\nInterpreter error: " << message << std::endl;
		throw std::exception();
	}
};