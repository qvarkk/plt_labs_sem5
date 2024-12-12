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

	std::stack<Token> stack;
	std::vector<std::pair<char, char>> equal_matrix;
	std::vector<std::pair<char, char>> less_matrix;
	std::vector<std::pair<char, char>> less_or_equal_matrix;
	std::vector<std::pair<char, char>> more_matrix;
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
		// Add initial ? Token at the beginning
		stack.push(Token("?", Type::SEPARATOR));
		Token current_token = lexer.get_next_token();

		while (stack.top().get_type() != Type::L || current_token.get_type() != Type::SEPARATOR) {
			while (shift_reduce(current_token))
				continue;
			current_token = lexer.get_next_token();
		}
		std::cout << "Fine" << std::endl;
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
			{'T', {"F(E)", "I", "C"}},
		};
	}

	bool shift_reduce(Token token) {
		if (has_relation_in_matrix(stack.top(), token, less_or_equal_matrix)) {
			token.set_precedence(Precedence::DUAL);
			stack.push(token);
			log_stack_state("shift (less or equal)");
		}
		else if (has_relation_in_matrix(stack.top(), token, equal_matrix)) {
			token.set_precedence(Precedence::EQUAL);
			stack.push(token);
			log_stack_state("shift (equal)");
		}
		else if (has_relation_in_matrix(stack.top(), token, less_matrix)) {
			token.set_precedence(Precedence::LESS);
			stack.push(token);
			log_stack_state("shift (less)");
		}
		else if (has_relation_in_matrix(stack.top(), token, more_matrix)) {
			reduce_stack();
			log_stack_state("reduce");
			return true;
		}
		else {
			error("invalid syntax (no such sequence of characters)");
		}
		return false;
	}

	void reduce_stack() {
		std::deque<Token> rules_deque = stack._Get_container();

		bool reduced = false;
		char rule = NULL;
		while (!rules_deque.empty()) {
			while (!rules_deque.empty() && rules_deque.front().get_precedence() != Precedence::LESS && rules_deque.front().get_precedence() != Precedence::DUAL) {
				rules_deque.pop_front();
			}

			rule = find_rule(get_deque_string(rules_deque));
			if (rule != NULL) {
				reduced = true;
				break;
			}

			rules_deque.pop_front();
		}

		for (int i = 0; i < rules_deque.size(); i++)
			stack.pop();

		if (!reduced)
			error("invalid syntax (no such rule)");

		Token reduced_token = Token("", static_cast<Type>(rule));
		if (has_relation_in_matrix(stack.top(), reduced_token, less_or_equal_matrix)) {
			reduced_token.set_precedence(Precedence::DUAL);
		}
		else if (has_relation_in_matrix(stack.top(), reduced_token, equal_matrix)) {
			reduced_token.set_precedence(Precedence::EQUAL);
		}
		else if (has_relation_in_matrix(stack.top(), reduced_token, less_matrix)) {
			reduced_token.set_precedence(Precedence::LESS);
		}
		else {
			error("invalid syntax (no such sequence of characters to reduce)");
		}

		stack.push(reduced_token);
	}

	bool has_relation_in_matrix(Token left, Token right, std::vector<std::pair<char, char>> matrix) {
		for (auto key_value_pairs : matrix) {
			if (key_value_pairs.first == left.get_char() && key_value_pairs.second == right.get_char())
				return true;
		}

		return false;
	}

	char find_rule(std::string _rule) {
		for (auto& rules : grammar) {
			for (auto& rule : rules.second) {
				if (rule == _rule) {
					return rules.first;
				}
			}
		}
		return NULL;
	}

	std::string get_stack_string() {
		std::stack<Token> tmp = stack;
		std::vector<char> stack_chars;
		while (!tmp.empty()) {
			stack_chars.push_back(tmp.top().get_char());
			stack_chars.push_back(tmp.top().get_precedence());
			tmp.pop();
		}

		std::reverse(stack_chars.begin(), stack_chars.end());
		return std::string(stack_chars.begin(), stack_chars.end());
	}
	
	std::string get_deque_string(std::deque<Token> deque) {
		std::string result = "";

		while (!deque.empty()) {
			result += deque.front().get_char();
			deque.pop_front();
		}

		return result;
	}

	void log_stack_state(std::string message) {
		std::cout << "Action: " << message << std::endl;
		std::cout << "Stack: \n\t";

		std::cout << get_stack_string() << std::endl;
		std::cout << std::endl;
	}

	void error(std::string message) {
		std::cout << "\nInterpreter error: " << message << std::endl;
		throw std::exception();
	}
};