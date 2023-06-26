#include <stdio.h>
#include <string.h>
#include "labyrinth.h"

#define UP            1   //0001 y ^ -
#define DOWN          2   //0010 y v +
#define LEFT          4   //0100 x < -
#define RIGHT         8   //1000 x > +

int direction(struct Labyrinth *, int, char **, int);

// обработчик команды движения
int direction(struct Labyrinth *lab, int count_args, char **command_args, int direction) {
	int num;
	// проверка, что число аргументов корректно
	switch(count_args) {
		case 0:
			// если нет аргументов, кол-во выполнений команды по умолчанию: 1
			num = 1;
			break;
		case 1:
			// проверка, что аргумент число
			if (sscanf(*command_args, "%d", &num) != 1) {
				fprintf(stderr, "arg is not a number\n");
				destroy_args(command_args, count_args);
				return 1;
			}
			// кол-во выполнений команды не может быть меньше 1
			if (num < 1) {
				fprintf(stderr, "number of command executed is less than 1\n");
				destroy_args(command_args, count_args);
				return 1;
			}
			break;
		default:
			fprintf(stderr, "incorrect number of args %d\n", count_args);
			destroy_args(command_args, count_args);
			return 1;
			
	}
	// перемещение на num шагов
	while (num > 0) {
		if (move(lab, direction) != 0) {
			destroy_args(command_args, count_args);
			return 1;
		}
		--num;
	}
	return 0;
}

int direction_left(struct Labyrinth *lab, int count_args, char **command_args) {
	if (direction(lab, count_args, command_args, LEFT) != 0) {
		return 1;
	}
	return 0;
}

int direction_right(struct Labyrinth *lab, int count_args, char **command_args) {
	if (direction(lab, count_args, command_args, RIGHT) != 0) {
		return 1;
	}
	return 0;
}

int direction_up(struct Labyrinth *lab, int count_args, char **command_args) {
	if (direction(lab, count_args, command_args, UP) != 0) {
		return 1;
	}
	return 0;
}

int direction_down(struct Labyrinth *lab, int count_args, char **command_args) {
	if (direction(lab, count_args, command_args, DOWN) != 0) {
		return 1;
	}
	return 0;
}

