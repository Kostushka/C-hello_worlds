#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
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

#define SUCCESS_MOVE 0
#define ERR_MOVE 1
#define NO_MODE 0
#define SET_MODE 1
#define DEFAULT_PREV_DIRECTION -1

#define MODE_SIZE(x) (int) (sizeof(x) / sizeof(x[0]))

// массив структур с режимами печати
struct Print_mode {
	char *mode;
	intptr_t value;
} mode[] = {
	{"each_step", EACH_STEP},	
	{"error", ERROR},	
	{"change_direction", CHANGE_DIRECTION},	
	{"target", TARGET},	
};

char *direction_str(int direction);

struct Parse_data *parse_move(int count_args, char **command_args) {
	struct Parse_data *parse_data = (struct Parse_data *) malloc(sizeof(struct Parse_data));
	if (parse_data == NULL) {
		perror("malloc");
		return NULL;
	}
	// проверка, что число аргументов корректно
	switch(count_args) {
		case 0:
			// если нет аргументов, кол-во выполнений команды по умолчанию: 1
			parse_data->arg = 1;
			break;
		case 1:
			// проверка, что аргумент число
			if (sscanf(*command_args, "%lu", &parse_data->arg) != 1) {
				fprintf(stderr, "arg is not a number\n");
				return NULL;
			}
			
			// кол-во выполнений команды не может быть меньше 1
			if (parse_data->arg < 1) {
				fprintf(stderr, "number of command executed is less than 1\n");
				return NULL;
			}
			break;
		default:
			fprintf(stderr, "incorrect number of args %d\n", count_args);
			return NULL;		
	}
	parse_data->handler = move;
	return parse_data;
}

// обработчик команды движения
int move(struct Context *context, struct Labyrinth *lab, uintptr_t arg, int direction) {
	// предыдущее направление сдвига точки
	context->prev_direction = context->curr_direction;
	// текущее направление сдвига точки
	context->curr_direction = direction;
	
	// перемещение на num шагов
	int num = (intptr_t) arg;
	while (num > 0) {
		if (step(context, lab, direction) != SUCCESS_MOVE) {
			// ошибка при сдвиге точки
			context->move_result = ERR_MOVE;
			// печать лабиринта
			print(context, lab);
			return 1;
		}
		// сдвиг точки прошел успешно
		context->move_result = SUCCESS_MOVE;
		--num;
		// печать лабиринта
		print(context, lab);
		// предыдущее направление сдвига точки не должно учитываться при текущих сдвигах, если их больше одного
		context->prev_direction = direction;
	}
	return 0;
}

struct Parse_data *parse_print_on(int count_args, char **command_args) {
	struct Parse_data *parse_data = (struct Parse_data *) malloc(sizeof(struct Parse_data));
	if (parse_data == NULL) {
		perror("malloc");
		return NULL;
	}
	static unsigned int command_arg;
	int mode_value;
	 
	// хэш для режимов печати
	struct Hash *hash_mode = hash_create(MODE_SIZE(mode));
	for (int i = 0; i < MODE_SIZE(mode); i++) {
		if (hash_add(hash_mode, mode[i].mode, (void *) mode[i].value) != 0) {
			return NULL;
		}
	}
	
	for (int i = 0; i < count_args; i++) {
		char *arg = command_args[i];
		char is_mode = SET_MODE;
		
		// если знак инверсии
		if (arg[0] == '~') {
			// отключение всех режимов печати
			if (arg[1] == '\0') {
				command_arg = NO_MODE;
				continue;
			}
			++arg;
			is_mode = NO_MODE;
		}
		
		// получение режима печати
		mode_value = (intptr_t) hash_find(hash_mode, arg);
		
		if (mode_value == 0) {
			fprintf(stderr, "%s mode not found\n", arg);
			return NULL;
		}
		
		if (is_mode) {
			// установка режима печати
			command_arg |= mode_value;
		} else {
			// отключение режима печати
			command_arg &= ~mode_value;
		}
	}
	parse_data->arg = command_arg;
	parse_data->handler = print_on;
	
	// очистить память выделенную под хэш
	hash_destroy(hash_mode);
	
	return parse_data;
}

int print_on(struct Context *context, struct Labyrinth *lab, uintptr_t arg, int command_name) {
	context->print_mode = (uintptr_t) arg;

	return 0;
}

void print(struct Context *context, struct Labyrinth *lab) {
	// отрисовать лабиринт при изменении направления сдвига точки
	if (context->print_mode & CHANGE_DIRECTION) {
		if (context->prev_direction != DEFAULT_PREV_DIRECTION && context->prev_direction != context->curr_direction) {
			printf("Изменение направления сдвига точки: %s\n", direction_str(context->curr_direction));
			print_lab(context, lab);
		}
	}
	// отрисовать лабиринт, если произошла ошибка при сдвиге точки
	if (context->print_mode & ERROR) {
		if (context->move_result == ERR_MOVE) {
			printf("Произошла ошибка при сдвиге точки, последнее допустимое состояние лабиринта:\n");
			print_lab(context, lab);
		}
	}
	// отрисовать лабиринт на каждом шаге
	if (context->print_mode & EACH_STEP) {
		if (context->move_result == SUCCESS_MOVE) {
			printf("Сдвиг точки\n");
			print_lab(context, lab);	
		}
	}
	// отрисовать лабиринт при достижении цели
	if (context->print_mode & TARGET) {
		if (context->traveler.x == context->target.x && context->traveler.y == context->target.y) {
			printf("Координаты точки совпали с координатами цели\n");
			print_lab(context, lab);
		}
	}
}

char *direction_str(int direction) {
	switch(direction) {
		case UP:
			return "вверх";
		case DOWN:
			return "вниз";
		case LEFT: 
			return "влево";
		case RIGHT:
			return "вправо";
		default:
			return "направление не определено";
	}
}
