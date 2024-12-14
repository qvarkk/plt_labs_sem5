#include <string>

enum Type {
	SEPARATOR = '?', 
	TERMINAL = '0',
	CONST = 'C', 
	FUNC = 'F', 
	ID = 'I', 
	T = 'T', 
	E = 'E', 
	S = 'S', 
	L = 'L', 
};

enum Precedence{
	EQUAL = '=', 
	NONE = ' ',
	LESS = '<', 
	MORE = '>', 
	DUAL = '@',
};

class Token {
private:
	std::string value;
	Type type;
	Precedence precedence;
	int triad_num;
public:
	Token(std::string value, Type type, Precedence precedence = NONE, int triad_num = 0) : 
		value(value), 
		type(type), 
		precedence(precedence), 
		triad_num(triad_num) {}

	std::string get_value() { return value; }
	Type get_type() { return type; }
	Precedence get_precedence() { return precedence; }
	int get_triad_num() { return triad_num; }

	char get_char() {
		if (this->type != Type::TERMINAL)
			return this->type;
		else
			return this->value[0];
	}

	void set_precedence(Precedence p) { precedence = p; }
	void set_triad_num(int n) { triad_num = n; }
};