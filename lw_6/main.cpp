#include "interpreter.cpp"

int main() {
	Lexer lexer;
	Interpreter interpreter(lexer);
	interpreter.interpret();
}