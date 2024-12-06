#include <string>

enum Type {
	ID = 'I', CONST = 'C', FUNC = 'F', TERMINAL = '0', END = '1'
};

class Token {
private:
	std::string value;
	Type type;
public:
	Token(std::string value, Type type) : value(value), type(type) {}

	std::string get_value() { return value; }
	Type get_type() { return type; }
};