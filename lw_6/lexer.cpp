#include <fstream>
#include <iostream>
#include "token.cpp"

class Lexer {
private:
	char current_char;
	std::ifstream file;

public:
	Lexer(std::string path = "input.txt") {
		file = std::ifstream(path);
		step_next_char();
	}

	~Lexer() {
		file.close();
	}

	Token get_next_token() {
		if (isalpha(current_char)) {
			std::string id = process_id_token();

			if (id == "sin" || id == "cos" || id == "sqr" || id == "sqrt")
				return Token(id, Type::FUNC);

			return Token(id, Type::ID);
		}
		else if (isdigit(current_char)) {
			std::string value = process_const_token();
			return Token(value, Type::CONST);
		}
		else if (current_char == '(' || current_char == ')' || current_char == '+' || current_char == '-') {
			Token token = Token(std::string(1, current_char), Type::TERMINAL);
			step_next_char();
			return token;
		}
		else if (current_char == EOF)
			return Token("?", Type::SEPARATOR);
		else
			error("invalid character");
	}

private:
	std::string process_id_token() {
		std::string id = "";

		while (isalpha(current_char)) {
			id += current_char;
			step_next_char();
		}

		return id;
	}

	std::string process_const_token() {
		std::string value = "";

		while (isdigit(current_char)) {
			value += current_char;
			step_next_char();
		}

		if (current_char == '.') {
			value += current_char;
			step_next_char();

			while (isdigit(current_char)) {
				value += current_char;
				step_next_char();
			}
		}

		return value;
	}

	void step_next_char() {
		if (current_char != EOF) {
			current_char = file.get();

			while (isspace(current_char)) {
				current_char = file.get();
			}
		}
	}

	void error(std::string message) {
		std::cout << "\nLexer error: " << message << std::endl;
		throw std::exception();
	}

	bool isspace(char c) {
		return c == ' ' || c == '\t' || c == '\n' || c == '\r';
	}

	bool isalpha(char c) {
		return c >= 'a' && c <= 'z';
	}

	bool isdigit(char c) {
		return c >= '0' && c <= '9';
	}
};