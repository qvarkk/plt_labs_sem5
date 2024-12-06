#include "interpreter.cpp"

	/*void interpret() {
		bool last_action_is_shift = true;

		stack = std::stack<char>();
		stack.push('?');
		
		char comp = '\0';
		while (true) {
			if (current_char != EOF) {
				comp = current_char;

				if (last_action_is_shift) {
					Tokens token = get_token();

					if (token != Tokens::Terminal)
						comp = token;
				}
			}
			else if (last_action_is_shift) {
				comp = '?';
			}

			if (has_relation_in_matrix(stack.top(), comp, equal_matrix)) {
				stack.push('=');
				stack.push(comp);
				last_action_is_shift = true;
				log_actions("shift (equal)");
			}
			else if (has_relation_in_matrix(stack.top(), comp, less_matrix)) {
				stack.push('<');
				stack.push(comp);
				last_action_is_shift = true;
				log_actions("shift (less)");
			}
			else if (has_relation_in_matrix(stack.top(), comp, more_matrix)) {
				reduce_stack(comp);
				last_action_is_shift = false;
				log_actions("convolute (more)");
			}
			else {
				error("interpret: Syntax error at " + std::string(1, current_char));
			}

			if (stack.top() == '?')
				break;
		}
	}

	
	void reduce_stack(char next_symbol) {
		char original_char = stack.top();
		char top = stack.top();
		std::vector<char> popped_symbols = { top };

		while (top != '<') {
			top = stack.top();
			popped_symbols.push_back(top);
			stack.pop();
		}
		
		char upper_level_rule, relation_symbol;

		if (original_char == 'C' || original_char == 'I' || original_char == 'F' ||
			original_char == ')' && popped_symbols.end()[-2] == 'F') {
			upper_level_rule = 'T';
		}
		else if (original_char == 'T') {
			upper_level_rule = 'E';
		}
		else if (original_char == 'E' || original_char == ')') {
			upper_level_rule = 'S';
		}
		else {
			upper_level_rule = '?';
		}

		if (has_relation_in_matrix(upper_level_rule, next_symbol, equal_matrix))
			relation_symbol = '=';
		else
			relation_symbol = '<';

		stack.push(relation_symbol);
		stack.push(upper_level_rule);
	}*/

int main() {
	Lexer lexer;
	Interpreter interpreter(lexer);
	interpreter.interpret();
}