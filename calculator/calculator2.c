#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX 100

struct Operation {
	int sum;
	int sign;
};

void calc_func(struct Operation *);
int getop(void);
int getch(void);
void ungetch(int c);

int main(void) {

	struct Operation string_to_parse = {0, 1};

	calc_func(&string_to_parse);

	printf("Sum: %d\n", string_to_parse.sum);
	
	return 0;
}

// 3 + 3 - 8 + 15 - 20

void calc_func(struct Operation *string_to_parse) {
	int op;
	while ((op = getop()) != EOF) {
		if (op == '+' || op == '-') {
			string_to_parse->sign = op == '+' ? 1 : -1;
		} else {
			string_to_parse->sum += string_to_parse->sign * op;
		}
	}
}

int getop(void) {
	int c;
	char s[MAX];
	
	while (isspace(c = getch()));

	if (!isdigit(c)) {
		return c;
	}

	if (isdigit(c)) {
		int i = 0;
		s[i] = c;
		while (isdigit(s[++i] = c = getch()));
		s[i] = '\0';
		
		if (c != EOF) {
			ungetch(c);
		}
		
		return atoi(s);
	}

	return c;
}

char buf[MAX];
int bufi = 0;

int getch(void) {
	return bufi > 0 ? buf[--bufi] : getchar();
}
void ungetch(int c) {
	if (bufi < MAX) {
		buf[bufi++] = c;
	} else {
		printf("buffer is full\n");
	}
}
