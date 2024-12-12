#include <string>

enum Type {
	ID = 'I', CONST = 'C', FUNC = 'F', T = 'T', E = 'E', S = 'S', L = 'L', SEPARATOR = '?', TERMINAL = '0', END = '1'
};

enum Precedence{
	NONE = ' ', LESS = '<', EQUAL = '=', MORE = '>', DUAL = '@'
};

class Token {
private:
	std::string value;
	Type type;
	Precedence precedence;
public:
	Token(std::string value, Type type, Precedence precedence = NONE) : value(value), type(type), precedence(precedence) {}

	std::string get_value() { return value; }
	Type get_type() { return type; }
	Precedence get_precedence() { return precedence; }

	char get_char() {
		if (this->type != Type::TERMINAL)
			return this->type;
		else
			return this->value[0];
	}

	void set_precedence(Precedence p) { precedence = p; }
};