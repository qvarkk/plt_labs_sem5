#include <iostream>
#include <fstream>

enum State { Awaiting, Slash, MLComment, SLComment, String, Char, BackslashString, BackslashChar, Asterisk };

int main() {
	std::ifstream input_file("input.cpp");
	std::ofstream output_file("output.cpp");
	State state = Awaiting;
	
	char c;

	while ((c = input_file.get()) != EOF) {
		switch (state) {
		case Awaiting:
			if (c == '/') {
				state = Slash;
			} else if (c == '\"') {
				state = String;
				output_file.put(c);
			} else if (c == '\'') {
				state = Char;
				output_file.put(c);
			} else {
				output_file.put(c);
			}
			break;
		case String:
			if (c == '\\') {
				state = BackslashString;
			} else if (c == '\"') {
				state = Awaiting;
			}
			output_file.put(c);
			break;
		case Char:
			if (c == '\\') {
				state = BackslashChar;
			} else if (c == '\'') {
				state = Awaiting;
			}
			output_file.put(c);
			break;
		case BackslashString:
			state = String;
			output_file.put(c);
			break;
		case BackslashChar:
			state = Char;
			output_file.put(c);
			break;
		case Slash:
			if (c == '*') {
				state = MLComment;
			}
			else if (c == '/') {
				state = SLComment;
			} 
			else {
				output_file.put('/');
				output_file.put(c);
				state = Awaiting;
			}
			break;
		case MLComment:
			if (c == '*') {
				state = Asterisk;
			}
			break;
		case SLComment:
			if (c == '\n' || c == '\r') {
				state = Awaiting;
				output_file.put(c);
			}
			break;
		case Asterisk:
			if (c == '/') {
				state = Awaiting;
				output_file.put(' ');
			}
			else if (c != '*') {
				state = MLComment;
			}
			break;
		}
	}

	input_file.close();
	output_file.close();

	std::cout << "Comments were removed!" << std::endl;
	return 0;
}