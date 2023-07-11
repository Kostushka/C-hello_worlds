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

#define SIZE(x) (int) (sizeof(x) / sizeof(x[0]))

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

struct Parse_direction {
	char *str;
	int num;
} parse_direction[] = {
	{"UP", UP},
	{"DOWN", DOWN},
	{"LEFT", LEFT},
	{"RIGHT", RIGHT},
};

char *direction_str(int direction);

struct Parse_data *parse_move(struct Context *context, int count_args, char *direction, char **command_args) {
	struct Parse_data *parse_data = (struct Parse_data *) malloc(sizeof(struct Parse_data));
	if (parse_data == NULL) {
		perror("malloc");
		return NULL;
	}
	// структура под аргументы
	move_args_t *move_arg = (move_args_t *) malloc(sizeof(move_args_t));
	if (move_arg == NULL) {
		perror("malloc");
		return NULL;
	}
	// значение направления по умолчанию
	move_arg->direction = 0;
	
	// записать направление
	for (int i = 0; i < SIZE(parse_direction); i++) {
		if (strcmp(direction, parse_direction[i].str) == 0) {
			move_arg->direction = parse_direction[i].num;
		}
	}
	
	if (move_arg->direction == 0) {
		fprintf(stderr, "%s is not correct direction\n", direction);
		return NULL;
	}
	
	move_arg->is_to_obstacle = 0;
	
	// проверка, что число аргументов корректно
	switch(count_args) {
		case 0:
			// если нет аргументов, кол-во выполнений команды по умолчанию: 1
			move_arg->num = 1;
			break;
		case 1:
			// проверка, что аргумент - флаг, означающий движение до упора в препятствие
			if (strcmp(*command_args, "to_obstacle") == 0) {
				move_arg->is_to_obstacle = 1;
				break;
			}
			
			// проверка, что аргумент число
			if (sscanf(*command_args, "%d", &move_arg->num) != 1) {
				fprintf(stderr, "arg is not a number\n");
				return NULL;
			}
			
			// кол-во выполнений команды не может быть меньше 1
			if (move_arg->num < 1) {
				fprintf(stderr, "number of command executed is less than 1\n");
				return NULL;
			}
			break;
		default:
			fprintf(stderr, "incorrect number of args %d\n", count_args);
			return NULL;		
	}
	parse_data->arg = move_arg;
	parse_data->handler = move;
	return parse_data;
}

// обработчик команды движения
int move(struct Context *context, struct Labyrinth *lab, void *arg) {
	move_args_t *move_args = (move_args_t *) arg;

	// предыдущее направление сдвига точки
	context->prev_direction = context->curr_direction;
	// текущее направление сдвига точки
	context->curr_direction = move_args->direction;

	// если команда движения до упора в препятствие
	if (move_args->is_to_obstacle) {
		move_args->num = 0;
		
		// перемещение до упора в препятствие
		while (1) {
			if (step(context, lab, move_args->direction) != SUCCESS_MOVE) {
				break;
			}
		
			// печать лабиринта
			print(context, lab);
			
			// предыдущее направление сдвига точки не должно учитываться при текущих сдвигах, если их больше одного
			context->prev_direction = move_args->direction;
		}
	}

	// перемещение на num шагов
	while (move_args->num > 0) {
		if (step(context, lab, move_args->direction) != SUCCESS_MOVE) {
			// ошибка при сдвиге точки
			context->move_result = ERR_MOVE;
			// печать лабиринта
			print(context, lab);
			return 1;
		}
		// сдвиг точки прошел успешно
		context->move_result = SUCCESS_MOVE;
		
		--move_args->num;
		
		// печать лабиринта
		print(context, lab);
		
		// предыдущее направление сдвига точки не должно учитываться при текущих сдвигах, если их больше одного
		context->prev_direction = move_args->direction;
	}
	return 0;
}

struct Parse_data *parse_print_on(struct Context *context, int count_args, char *command_name, char **command_args) {
	struct Parse_data *parse_data = (struct Parse_data *) malloc(sizeof(struct Parse_data));
	if (parse_data == NULL) {
		perror("malloc");
		return NULL;
	}
	
	// структура под аргументы
	// print_args_t *print_arg = (print_args_t *) malloc(sizeof(print_args_t));
	// if (print_arg == NULL) {
		// perror("malloc");
		// return NULL;
	// }
	
	int mode_value;
	 
	// хэш для режимов печати
	struct Hash *hash_mode = hash_create(SIZE(mode));
	for (int i = 0; i < SIZE(mode); i++) {
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
				context->print_mode = NO_MODE;
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
			context->print_mode |= mode_value;
		} else {
			// отключение режима печати
			context->print_mode &= ~mode_value;
		}
	}
	// print_arg->mode = context->print_mode;
	// parse_data->arg = print_arg;
	parse_data->arg = NULL;
	parse_data->handler = print_on;
	
	// очистить память выделенную под хэш
	hash_destroy(hash_mode);
	
	return parse_data;
}

int print_on(struct Context *context, struct Labyrinth *lab, void *arg) {
	// context->print_mode = ((print_args_t *)arg)->mode;
	// для функции печати команды нет обработчика исполнения
	if (arg != NULL) {
		return 1;
	}
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
			context->target.x = -1;
			context->target.y = -1;
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


