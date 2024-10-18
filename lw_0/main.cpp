#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main() {
	std::ifstream input_file("input.cpp");
	std::ofstream output_file("output.cpp");

	if (!input_file) {
		std::cerr << "Couldn't open the input.cpp file!";
	}

	if (!output_file) {
		std::cerr << "Couldn't open the output.cpp file!";
	}

	char ch1, ch2;
	std::vector<char> file_chars;


	ch1 = input_file.get();
	while (EOF != (ch2 = input_file.get())) {
		if (ch1 == '/' && ch2 == '*') {
			while (EOF != (ch2 = input_file.get())) {

				if (ch1 == '*' && ch2 == '/') {
					break;
				}

				ch1 = ch2;
			}
		} else {
			if (ch1 != '/' && ch2 != '*')
				output_file.put(ch1);
		}

		ch1 = ch2;
	}
	output_file.put(ch1);

	input_file.close();
	output_file.close();

	std::cout << "Removed comments successfully!" << std::endl;
	return 0;
}