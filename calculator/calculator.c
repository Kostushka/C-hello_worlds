#include <stdio.h>

struct Operation {
	int op1;
	int op2;
	char sign;
};

int calc_func(struct Operation *);

int main(void) {

	struct Operation string_to_parse;

	int result = calc_func(&string_to_parse);

	printf("Sum: %d\n", result);
	
	return 0;
}

int calc_func(struct Operation *string_to_parse) {
	int a, sum = 0;
	
	while(1) {

		printf("Введите арифметическую операцию:\n");
		a = scanf("%d %c %d", &string_to_parse->op1, &string_to_parse->sign, &string_to_parse->op2);

		if (a == -1) break;

		// обработка некорректного ввода
		if (string_to_parse->sign != '+' && string_to_parse->sign != '-') {
			printf("invalid operator, try again\n");
			continue;
		}
		if (a != 3) {
			printf("invalid value, try again\n");
			continue;
		}

		int sign = string_to_parse->sign == '+' ? 1 : -1;

		sum += string_to_parse->op1 + sign * string_to_parse->op2;
	}

	return sum;
}
