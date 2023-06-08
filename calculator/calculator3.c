#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX 100

struct Operation {
	int sum;
	int sign;
};

void parse_func(struct Operation *);
int getop(char *);

int main(void) {

	struct Operation string_to_parse = {0, 1};

	parse_func(&string_to_parse);

	printf("Sum: %d\n", string_to_parse.sum);
	
	return 0;
}

// 3 + 3 - 8 + 15 - 20

void parse_func(struct Operation *string_to_parse) {
	int op;
	char is_int = 0;
	int count_serial_int = 0;
	
	while ((op = getop(&is_int)) != EOF) {
		switch(op) {
			case '#':
				printf("%d\n", string_to_parse->sum);
				continue;
			case '+':
			case '-':
				count_serial_int = 0;
				string_to_parse->sign = op == '+' ? 1 : -1;
				break;
			default:
				if (is_int) {
					++count_serial_int;
					if (count_serial_int > 1) {
						printf("incorrect transaction entry: you should use operation sign between integer\n");
						return;
					}
					string_to_parse->sum += string_to_parse->sign * op;
				}
				break;
		}
		// if (op == '#') {
			// printf("%d\n", string_to_parse->sum);
			// continue;
		// }
		// if (op == '+' || op == '-') {
			// count_serial_int = 0;
			// string_to_parse->sign = op == '+' ? 1 : -1;
		// }
		// if (is_int) {
			// ++count_serial_int;
			// if (count_serial_int > 1) {
				// printf("incorrect transaction entry: you should use operation sign between integer\n");
				// return;
			// }
			// string_to_parse->sum += string_to_parse->sign * op;
		// }
	}
}

int getop(char *is_int) {
	int c;
	char s[MAX];
	
	while (isspace(c = getc(stdin)));

	if (!isdigit(c)) {
		*is_int = 0;
		return c;
	}

	int i;
	for (i = 0; isdigit(c); i++) {
		if (i == MAX - 1) {
			s[i] = '\0';
			printf("not enough space to write the number\n");
			return EOF;
		} 
		s[i] = c;
		c = getc(stdin);
	}

	s[i] = '\0';

	if (c != EOF) {
		ungetc(c, stdin);
		*is_int = 1;
		return atoi(s);
	}

	return c;
}
