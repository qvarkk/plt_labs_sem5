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
	std::vector<std::string> initialized_variables;
	std::vector<std::tuple<char, std::string, std::string>> triads;

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

		while (true) {
			if (stack.top().get_type() == Type::SEPARATOR && current_token.get_type() == Type::SEPARATOR)
				break;

			while (shift_reduce(current_token))
				continue;

			current_token = lexer.get_next_token();

			if (stack.top().get_type() == Type::ID && current_token.get_value() != "(") {
				bool found = false;
				for (auto variable : initialized_variables) {
					if (stack.top().get_value() == variable) {
						found = true;
						break;
					}
				}

				if (!found)
					error("Variable with name " + stack.top().get_value() + " is not initialized");
			}
		}

		std::cout << "Fine" << std::endl;
		output_triads();
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
		std::string string_rule = "";
		while (!rules_deque.empty()) {
			while (!rules_deque.empty() && rules_deque.front().get_precedence() != Precedence::LESS && rules_deque.front().get_precedence() != Precedence::DUAL) {
				rules_deque.pop_front();
			}

			string_rule = get_deque_string(rules_deque);
			rule = find_rule(string_rule);
			if (rule != NULL) {
				reduced = true;
				break;
			}

			rules_deque.pop_front();
		}

		if (!reduced)
			error("invalid syntax (no such rule)");

		for (int i = 0; i < rules_deque.size(); i++) {
			if (rule == 'S' && stack.top().get_type() == Type::ID)
				initialized_variables.push_back(stack.top().get_value());

			stack.pop();
		}

		Token reduced_token = Token("", static_cast<Type>(rule));
		form_triad(reduced_token, rules_deque, string_rule);

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

	void form_triad(Token& reduced_token, std::deque<Token> tokens_sequence, std::string rule) {
		if (rule == "C") {	// C->T: C(n, ? )
			triads.push_back({'C', tokens_sequence[0].get_value(), "?"});
			reduced_token.set_triad_num(triads.size());
		}
		else if (rule == "I") {	// I -> T: V(n, ?)
			triads.push_back({ 'V', tokens_sequence[0].get_value(), "?"});
			reduced_token.set_triad_num(triads.size());
		}
		else if (rule == "F(E)") {	// F(E) -> T: S,Q,R,T(^e, ?)
			char function_triad_char;
			if (tokens_sequence[0].get_value() == "sin")
				function_triad_char = 'S';
			else if (tokens_sequence[0].get_value() == "cos")
				function_triad_char = 'K';
			else if (tokens_sequence[0].get_value() == "sqr")
				function_triad_char = 'Q';
			else
				function_triad_char = 'R';

			triads.push_back({ function_triad_char, "^" + std::to_string(tokens_sequence[2].get_triad_num()), "?"});
			reduced_token.set_triad_num(triads.size());
		}
		else if (rule == "T") {	// Just move triad_num to new symbol
			reduced_token.set_triad_num(tokens_sequence[0].get_triad_num());
		}
		else if (rule == "E+T") {	// E+T -> E: +(^e, ^t)
			triads.push_back({ '+', "^" + std::to_string(tokens_sequence[0].get_triad_num()), "^" + std::to_string(tokens_sequence[2].get_triad_num())});
			reduced_token.set_triad_num(triads.size());
		}
		else if (rule == "E-T") {	// E-T -> E: -(^e, ^t)
			triads.push_back({ '-', "^" + std::to_string(tokens_sequence[0].get_triad_num()), "^" + std::to_string(tokens_sequence[2].get_triad_num())});
			reduced_token.set_triad_num(triads.size());
		}
		else if (rule == "I(E)") {	// I(E) -> S: V(i, ?)  =(^v, ^e)
			triads.push_back({ 'V', tokens_sequence[0].get_value(), "?"});
			triads.push_back({ '=', "^" + std::to_string(triads.size()), "^" + std::to_string(tokens_sequence[2].get_triad_num())});
		}
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
		std::cout << "Action:\t" << message << std::endl;
		std::cout << "Stack:\t\b" << get_stack_string() << std::endl;

		std::cout << std::endl;
	}

	void output_triads() {
		int i = 1;
		for (auto x : triads) {
			std::cout << i << ". " << std::get<0>(x) << "(" << std::get<1>(x) << ", " << std::get<2>(x) << ")\n";
			i++;
		}
	}

	void error(std::string message) {
		std::cout << "\nInterpreter error: " << message << std::endl;
		throw std::exception();
	}
};