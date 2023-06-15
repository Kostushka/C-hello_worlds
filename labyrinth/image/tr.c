#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **create_arr(int);

int main(void) {
	char **arr = create_arr(5);
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			printf("|%c|", arr[i][j]);
		}
		putchar('\n');
	}
	return 0;
}

char **create_arr(int a) {
	// массив указателей на массивы строк
	char **keytab = (char **) calloc(a, sizeof(char *));
	int i, j;
	for (i = 0; i < 5; i++) {
		// массив строки
		keytab[i] = (char *) malloc(sizeof(char) * a);
		for (j = 0; j < 5; j++) {
			keytab[i][j] = 'a';
		}
	}
	return keytab;
}
