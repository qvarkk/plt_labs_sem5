#include <iostream>
#include <fstream>

enum State { Awaiting, Slash, Comment, Asterisk };

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
			} else {
				output_file.put(c);
			}
			break;
		case Slash:
			if (c == '*') {
				state = Comment;
			} else if (c == '/') {
				output_file.put('/');
			} else {
				output_file.put('/');
				output_file.put(c);
				state = Awaiting;
			}
			break;
		case Comment:
			if (c == '*') {
				state = Asterisk;
			}
			break;
		case Asterisk:
			if (c == '/') {
				state = Awaiting;
			} else if (c != '*') {
				state = Comment;
			}
			break;
		}
	}

	input_file.close();
	output_file.close();

	std::cout << "Comments were removed!" << std::endl;
	return 0;
}