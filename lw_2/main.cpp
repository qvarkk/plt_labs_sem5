#include <iostream>
#include <fstream>
#include <vector>

enum State { Normal, Slash, MLComment, SLComment, String, 
			Char, BackslashString, BackslashChar, Asterisk,
			Blocked, Number, StartDot, Decimal, BeforeE, E, Sign, Long, Float };

bool inAlpha(char);
bool inBeta(char);
bool inGamma(char);

int main() {
	std::ifstream input_file("input.cpp");
	std::ofstream output_file("output.cpp");
	State state = Normal;

	char c;

	while ((c = input_file.get()) != EOF) {
		switch (state) {
		case Normal:
			if (c == '\'') {
				state = Char;
			} else if (c == '\"') {
				state = String;
			} else if (c == '.') {
				state = StartDot;
				output_file.put(c);
			} else if (c == '/') {
				state = Slash;
			} else if (inAlpha(c)) {
				state = Blocked;
			} else if (inGamma(c)) {
				state = Number;
				output_file.put(c);
			}
			break;
		case String:
			if (c == '\\') {
				state = BackslashString;
			} else if (c == '\"') {
				state = Normal;
			}
			break;
		case Char:
			if (c == '\\') {
				state = BackslashChar;
			} else if (c == '\'') {
				state = Normal;
			}
			break;
		case BackslashString:
			state = String;
			break;
		case BackslashChar:
			state = Char;
			break;
		case Slash:
			if (c == '\'') {
				state = Char;
			} else if (c == '\"') {
				state = String;
			} else if (c == '.') {
				state = StartDot;
				output_file.put(c);
			} else if (c == '/') {
				state = SLComment;
			} else if (c == '*') {
				state = MLComment;
			} else if (inAlpha(c)) {
				state = Blocked;
			} else if (inGamma(c)) {
				state = Number;
				output_file.put(c);
			}
			break;
		case MLComment:
			if (c == '*') {
				state = Asterisk;
			}
			break;
		case SLComment:
			if (c == '\n' || c == '\r') {
				state = Normal;
			}
			break;
		case Asterisk:
			if (c == '/') {
				state = Normal;
			} else if (c == '*') {
				continue;
			} else {
				state = MLComment;
			}
			break;
		case Blocked:
			if (c == '/') {
				state = Slash;
			} else if (inBeta(c)) {
				state = Normal;
			}
			break;
		case Number:
			if (c == '.') {
				state = Decimal;
				output_file.put(c);
			} else if (c == 'e') {
				state = BeforeE;
				output_file.put(c);
			} else if (inGamma(c)) {
				output_file.put(c);
			} else if (c == '\"') {
				state = String;
				output_file << " ERROR\n";
			} else if (c == '\'') {
				state = Char;
				output_file << " ERROR\n";
			} else if (c == '/') {
				state = Slash;
				output_file << " ERROR\n";
			} else if (inAlpha(c)) {
				state = Blocked;
				output_file << c << " ERROR\n";
			} else {
				state = Normal;
				output_file << c << " ERROR\n";
			}
			break;
		case StartDot:
			if (inGamma(c)) {
				state = Decimal;
				output_file.put(c);
			} else if (c == '\"') {
				state = String;
				output_file << c << " ERROR\n";
			} else if (c == '\'') {
				state = Char;
				output_file << c << " ERROR\n";
			} else if (c == '/') {
				state = Slash;
				output_file << c << " ERROR\n";
			} else if (inAlpha(c)) {
				state = Blocked;
				output_file << c << " ERROR\n";
			} else {
				state = Normal;
				output_file << c << " ERROR\n";
			}
			break;
		case Decimal:
			if (inGamma(c)) {
				output_file.put(c);
			} else if (c == '.') {
				state = Blocked;
				output_file << c << " ERROR\n";
			} else if (c == 'L' || c == 'l') {
				state = Long;
				output_file.put(c);
			} else if (c == 'F' || c == 'f') {
				state = Float;
				output_file.put(c);
			} else if (c == 'e') {
				state = BeforeE;
				output_file.put(c);
			} else if (c == '\"') {
				state = String;
				output_file << " DOUBLE\n";
			} else if (c == '\'') {
				state = Char;
				output_file << " DOUBLE\n";
			} else if (c == '/') {
				state = Slash;
				output_file << " DOUBLE\n";
			} else if (inAlpha(c)) {
				state = Blocked;
				output_file << c << " ERROR\n";
			} else {
				state = Normal;
				output_file << " DOUBLE\n";
			}
			break;
		case BeforeE:
			if (c == '+' || c == '-') {
				state = Sign;
				output_file.put(c);
			} else if (inGamma(c)) {
				state = E;
				output_file.put(c);
			} else if (c == '\"') {
				state = String;
				output_file << c << " ERROR\n";
			} else if (c == '\'') {
				state = Char;
				output_file << c << " ERROR\n";
			} else if (c == '/') {
				state = Slash;
				output_file << c << " ERROR\n";
			} else if (inAlpha(c)) {
				state = Blocked;
				output_file << c << " ERROR\n";
			} else {
				state = Normal;
				output_file << c << " ERROR\n";
			}
			break;
		case Sign:
			if (inGamma(c)) {
				state = E;
				output_file.put(c);
			} else if (c == '\"') {
				state = String;
				output_file << c << " ERROR\n";
			} else if (c == '\'') {
				state = Char;
				output_file << c << " ERROR\n";
			} else if (c == '/') {
				state = Slash;
				output_file << c << " ERROR\n";
			} else if (inAlpha(c)) {
				state = Blocked;
				output_file << c << " ERROR\n";
			} else {
				state = Normal;
				output_file << c << " ERROR\n";
			}
			break;
		case E:
			if (inGamma(c)) {
				output_file.put(c);
			} else if (c == 'L' || c == 'l') {
				state = Long;
				output_file.put(c);
			} else if (c == 'F' || c == 'f') {
				state = Float;
				output_file.put(c);
			} else if (c == '\"') {
				state = String;
				output_file << " DOUBLE\n";
			} else if (c == '\'') {
				state = Char;
				output_file << " DOUBLE\n";
			} else if (c == '/') {
				state = Slash;
				output_file << " DOUBLE\n";
			} else if (inAlpha(c)) {
				state = Blocked;
				output_file << c << " ERROR\n";
			} else {
				state = Normal;
				output_file << " DOUBLE\n";
			}
			break;
		case Long:
			if (inGamma(c)) {
				state = Blocked;
				output_file << c << " ERROR\n";
			} else if (c == '\"') {
				state = String;
				output_file << " LONG DOUBLE\n";
			} else if (c == '\'') {
				state = Char;
				output_file << " LONG DOUBLE\n";
			} else if (c == '/') {
				state = Slash;
				output_file << " LONG DOUBLE\n";
			} else if (inAlpha(c)) {
				state = Blocked;
				output_file << c << " ERROR\n";
			} else {
				state = Normal;
				output_file << " LONG DOUBLE\n";
			}
			break;
		case Float:
			if (inGamma(c)) {
				state = Blocked;
				output_file << c << " ERROR\n";
			} else if (c == '\"') {
				state = String;
				output_file << " FLOAT\n";
			} else if (c == '\'') {
				state = Char;
				output_file << " FLOAT\n";
			} else if (c == '/') {
				state = Slash;
				output_file << " FLOAT\n";
			} else if (inAlpha(c)) {
				state = Blocked;
				output_file << c << " ERROR\n";
			} else {
				state = Normal;
				output_file << " FLOAT\n";
			}
			break;
			break;
		}
	}

	input_file.close();
	output_file.close();

	std::cout << "Felina" << std::endl;
	return 0;
}

bool inAlpha(char c) {
	return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_';
}

bool inBeta(char c) {
	return !inAlpha(c) && !inGamma(c);
}

bool inGamma(char c) {
	return c >= '0' && c <= '9';
}