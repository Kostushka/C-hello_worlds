#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "labyrinth.h"

#define UP                  1   //00000001 y ^ -
#define DOWN                2   //00000010 y v +
#define LEFT                4   //00000100 x < -
#define RIGHT               8   //00001000 x > +

#define EACH_STEP          16   //00010000
#define ERROR              32   //00100000
#define CHANGE_DIRECTION   64   //01000000
#define TARGET            128   //10000000

#define MODE_SIZE(x) (int) (sizeof(x) / sizeof(x[0]))

// массив структур с режимами печати
struct Mode {
	char *mode;
	intptr_t value;
} mode[] = {
	{"each_step", EACH_STEP},	
	{"error", ERROR},	
	{"change_direction", CHANGE_DIRECTION},	
	{"target", TARGET},	
};

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
	int mode_value;
	
	// хэш для режимов печати
	struct Hash *hash_mode = hash_create(MODE_SIZE(mode));
	for (int i = 0; i < MODE_SIZE(mode); i++) {
		if (hash_add(hash_mode, mode[i].mode, (void *) mode[i].value) != 0) {
			return 1;
		}
	}
	for (int i = 0; i < count_args; i++) {
		int is_invers = 0;
		char *arg;
		char is_mode;
		
		// если знак инверсии
		if (command_args[i][0] == '~') {
			// отключение всех режимов печати
			if (command_args[i][1] == '\0') {
				print_mode = 0;
				continue;
			}
			is_invers = 1;
		} 
		
		if (is_invers) {
			arg = &command_args[i][1];
			is_mode = 0;
		} else {
			arg = command_args[i];
			is_mode = 1;
		}
		
		// получение режима печати
		mode_value = (intptr_t) hash_find(hash_mode, arg);
		
		if (mode_value == 0) {
			fprintf(stderr, "%s mode not found\n", arg);
			return 1;
		}
		
		if (is_mode) {
			// установка режима печати
			print_mode |= mode_value;
		} else {
			// отключение режима печати
			print_mode &= ~mode_value;
		}
	}
	// очистить память выделенную под хэш
	hash_destroy(hash_mode);
	printf("print_mode: %o\n", print_mode);
	return 0;
}

