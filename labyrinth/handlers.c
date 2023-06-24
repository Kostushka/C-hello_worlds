#include <stdio.h>
#include <string.h>
#include "labyrinth.h"

#define UP            1   //0001 y ^ -
#define DOWN          2   //0010 y v +
#define LEFT          4   //0100 x < -
#define RIGHT         8   //1000 x > +

// обработчик команды движения
int direction(struct Labyrinth *lab, int count_args, char **command_args) {
	// проверка, что число аргументов корректно
	if (count_args < 0 || count_args > 1) {
		fprintf(stderr, "incorrect number of args %d\n", count_args);
		return 1;
	}
	// если нет аргументов, кол-во выполнений команды по умолчанию: 1
	if (count_args == 0) {
		if (move(lab, UP) != 0) {
			return 1;
		}
		return 0;
	}
	int num;
	// проверка, что аргумент число
	if (sscanf(*command_args, "%d", &num) != 1) {
		fprintf(stderr, "arg is not a number\n");
		destroy_args(command_args, count_args);
		return 1;
	}
	// перемещение на num шагов
	while (num > 0) {
		if (move(lab, UP) != 0) {
			return 1;
		}
		--num;
	}
	return 0;
}

int direction_left(char *command_name) {
	
}

