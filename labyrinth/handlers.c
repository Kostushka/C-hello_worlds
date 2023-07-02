#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "labyrinth.h"

enum direction {
	UP = 1,                  //00000001 y ^ -
	DOWN = 2,                //00000010 y v +
	LEFT = 4,                //00000100 x < -
	RIGHT = 8                //00001000 x > +
};

enum mode {
	EACH_STEP = 16,          //00010000
	ERROR = 32,              //00100000
	CHANGE_DIRECTION = 64,   //01000000
	TARGET = 128             //10000000
};

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

int direction(struct Context *, struct Labyrinth *, int, char **, int);

// обработчик команды движения
int direction(struct Context *context, struct Labyrinth *lab, int count_args, char **command_args, int direction) {
	static int prev_direction; // 0
	context->prev_direction = prev_direction;
	context->curr_direction = direction;
	prev_direction = direction;
	printf("prev: %d\ncurr: %d\nmove res: %d\n", context->prev_direction, context->curr_direction, context->move_result);
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
		if (move(context, lab, direction) != 0) {
			context->move_result = 1;
			destroy_args(command_args, count_args);
			return 1;
		}
		context->move_result = 0;
		--num;
	}
	return 0;
}

int direction_left(struct Context *context, struct Labyrinth *lab, int count_args, char **command_args) {
	return direction(context, lab, count_args, command_args, LEFT);
}

int direction_right(struct Context *context, struct Labyrinth *lab, int count_args, char **command_args) {
	return direction(context, lab, count_args, command_args, RIGHT);
}

int direction_up(struct Context *context, struct Labyrinth *lab, int count_args, char **command_args) {
	return direction(context, lab, count_args, command_args, UP);
}

int direction_down(struct Context *context, struct Labyrinth *lab, int count_args, char **command_args) {
	return direction(context, lab, count_args, command_args, DOWN);
}

int print_on(struct Context *context, struct Labyrinth *lab, int count_args, char **command_args) {
	int mode_value;
	
	// хэш для режимов печати
	struct Hash *hash_mode = hash_create(MODE_SIZE(mode));
	for (int i = 0; i < MODE_SIZE(mode); i++) {
		if (hash_add(hash_mode, mode[i].mode, (void *) mode[i].value) != 0) {
			return 1;
		}
	}
	for (int i = 0; i < count_args; i++) {
		char *arg = command_args[i];
		char is_mode = 1;
		
		// если знак инверсии
		if (arg[0] == '~') {
			// отключение всех режимов печати
			if (arg[1] == '\0') {
				context->print_mode = 0;
				continue;
			}
			++arg;
			is_mode = 0;
		}
		
		// получение режима печати
		mode_value = (intptr_t) hash_find(hash_mode, arg);
		
		if (mode_value == 0) {
			fprintf(stderr, "%s mode not found\n", arg);
			return 1;
		}
		
		if (is_mode) {
			// установка режима печати
			context->print_mode |= mode_value;
		} else {
			// отключение режима печати
			context->print_mode &= ~mode_value;
		}
	}
	// очистить память выделенную под хэш
	hash_destroy(hash_mode);
	printf("print_mode: %o\n", context->print_mode);
	return 0;
}

