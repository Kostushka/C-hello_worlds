#include <stdio.h>
#include <string.h>
#include "labyrinth.h"

#define UP                  1   //00000001 y ^ -
#define DOWN                2   //00000010 y v +
#define LEFT                4   //00000100 x < -
#define RIGHT               8   //00001000 x > +

#define EACH_STEP          16   //00010000
#define ERROR              32   //00100000
#define CHANGE_DIRECTION   64   //01000000
#define TARGET            128   //10000000

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
	return direction(lab, count_args, command_args, LEFT);
}

int direction_right(struct Labyrinth *lab, int count_args, char **command_args) {
	return direction(lab, count_args, command_args, RIGHT);
}

int direction_up(struct Labyrinth *lab, int count_args, char **command_args) {
	return direction(lab, count_args, command_args, UP);
}

int direction_down(struct Labyrinth *lab, int count_args, char **command_args) {
	return direction(lab, count_args, command_args, DOWN);
}

int print_on(struct Labyrinth *lab, int count_args, char **command_args) {
	unsigned int print_mode = 0;
	for (int i = 0; i < count_args; i++) {
		if (command_args[i][0] == '~') {
			if (strcmp(command_args[i], "~") == 0) {
				print_mode &= ~(EACH_STEP | ERROR | CHANGE_DIRECTION | TARGET);
			} else if (strcmp(command_args[i], "~each_step") == 0) {
				print_mode &= ~EACH_STEP;
			} else if (strcmp(command_args[i], "~error") == 0) {
				print_mode &= ~ERROR;
			} else if (strcmp(command_args[i], "~change_direction") == 0) {
				print_mode &= ~CHANGE_DIRECTION;
			} else if (strcmp(command_args[i], "~target") == 0) {
				print_mode &= ~TARGET;
			} else {
				fprintf(stderr, "incorrect name of args for command print %s\n", command_args[i]);
				return 1;
			}
		} else if (strcmp(command_args[i], "each_step") == 0) {
			print_mode |= EACH_STEP; 
		} else if (strcmp(command_args[i], "error") == 0) {
			print_mode |= ERROR;
		} else if (strcmp(command_args[i], "change_direction") == 0) {
			print_mode |= CHANGE_DIRECTION;
		} else if (strcmp(command_args[i], "target") == 0) {
			print_mode |= TARGET;
		} else {
			fprintf(stderr, "incorrect name of args for command print %s\n", command_args[i]);
			return 1;
		}
	}
	printf("print_mode: %o\n", print_mode);
	return 0;
}

